/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "foc.h"
#include "math.h"
#include "stdio.h"
#include "pid.h"
#include "scope.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
#define PERIODTIME PWM_MAX_DUTY
/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder);
static void transform(struct Foc *foc);
static void tempScope(float data1, float data2, float data3, const float max);
static void calculateSVPWM(struct Foc *foc);
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
Foc foc = {
    .init = focInit,
};

parameterPID_t *currentLoopD;
parameterPID_t *currentLoopQ;
parameterPID_t *position;

float UrefAngleTemp = 0;
// 计算常量
float pi;
float angle2Radian;
float radian2Angle;
float valueOfSin;
float sqrtof3;
/*------------------------------------------------------*/
/*                       函数定义                       */
/*======================================================*/
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder)
{
    //? PID参数
    float iKp = 0.6f;
    float iKi = 0.008f;
    float iKd = 0.002f;
    float threshold = 100;
    //! Foc结构体初始化
    foc->transform = transform;
    foc->svpwmAction = calculateSVPWM;

    foc->driver = driver;
    foc->sensor = driver->sensor;
    foc->encoder = encoder;

    foc->currentA = 0;
    foc->currentB = &(driver->sensor->current2);
    foc->currentC = &(driver->sensor->current1);

    foc->voltage = &(driver->sensor->voltage);

    foc->angle = &(encoder->absAngle);

    foc->cycleGain = 7;

    foc->targetCurrent = 5;
    foc->targetAngle = 180;

    //? PID结构初始化
    currentLoopD = _parameterPID(iKp, iKi, iKd, threshold, &foc->Id);
    currentLoopQ = _parameterPID(iKp, iKi, iKd, threshold, &foc->Iq);
    position = _parameterPID(1, 0.09, 0.0001, 60, foc->angle);

    currentLoopD->setPoint = 0;
    currentLoopQ->setPoint = 0.1;

    foc->commandId = &currentLoopD->controllerOutput;
    foc->commandIq = &currentLoopQ->controllerOutput;
    //! Foc结构体初始化

    // 计算常量初始化
    pi = acos(-1.f);
    angle2Radian = pi / 180;
    radian2Angle = 180 / pi;
    valueOfSin = sin(pi / 3); // sin(2*pi/3) = sin(pi/3)
    sqrtof3 = sqrt(3);

    //? push data into scope
    scopePushValue(foc->angle, sizeof(*foc->angle), "angle", false);

    // scopePushValue(&foc->currentA, sizeof(foc->currentA), "currentA", false);
    // scopePushValue(foc->currentB, sizeof(*foc->currentB), "currentB", false);
    // scopePushValue(foc->currentC, sizeof(*foc->currentC), "currentC", false);

    // scopePushValue(&foc->alpha, sizeof(foc->alpha), "Alpha", false);
    // scopePushValue(&foc->beta, sizeof(foc->beta), "Beta", false);

    scopePushValue(&foc->Id, sizeof(foc->alpha), "Id", false);
    scopePushValue(&foc->Iq, sizeof(foc->beta), "Iq", false);

    // scopePushValue(&foc->commandAlpha, sizeof(foc->commandAlpha), "commandAlpha", false);
    // scopePushValue(&foc->commandBeta, sizeof(foc->commandBeta), "commandBeta", false);

    scopePushValue(&UrefAngleTemp, sizeof(UrefAngleTemp), "UrefAngle", false);
    // scopePushValue(&foc->UrefAngle, sizeof(foc->UrefAngle), "UrefAngle", false);
    scopePushValue(&foc->Uref, sizeof(foc->Uref), "Uref", false);
}
/*------------------------------*/
/*		     坐标变换     	     */
/*==============================*/
static inline void clarkTransform(struct Foc *foc)
{
    foc->alpha = foc->currentA - (*foc->currentB / 2) - (*foc->currentC / 2); // cos(2*pi/3) = -0.5 | no need to consider direction
    foc->beta = valueOfSin * (*foc->currentB - *foc->currentC);
}
static inline void parkTransform(struct Foc *foc, const float radian)
{
    float theta = foc->cycleGain * radian;

    foc->Id = foc->alpha * cos(theta) + foc->beta * sin(theta);
    foc->Iq = -foc->alpha * sin(theta) + foc->beta * cos(theta);

    // tempScope(foc->Id, foc->Iq, 0, 2);
}
//? 反Park变换
static inline void invertedParkTransform(struct Foc *foc, const float radian)
{
    float theta = radian;
    foc->commandAlpha = *foc->commandId * cos(theta) - *foc->commandIq * sin(theta);
    foc->commandBeta = *foc->commandId * sin(theta) + *foc->commandIq * cos(theta);

    // tempScope(foc->commandAlpha, foc->commandBeta, 0, 2);
}
static inline void angleCalculate(struct Foc *foc)
{
    foc->Uref = pow((foc->commandAlpha * foc->commandAlpha) + (foc->commandBeta * foc->commandBeta), 1.f / 2.f); // 计算Uref
    if (foc->Uref < 0.001)
        foc->Uref = 0;
    // 计算Uref的角度
    if (foc->commandAlpha > 0) // 1、4象限
    {
        foc->UrefAngle = atan(foc->commandBeta / foc->commandAlpha) * radian2Angle;
        if (foc->UrefAngle < 0)
        {
            foc->UrefAngle += 360;
        }
        // ips114_showfloat(0, 2, foc->UrefAngle, 3, 3);
    }
    else if (foc->commandAlpha < 0)
    {
        if (foc->commandBeta > 0) // 2象限
        {
            float tempAngle = acos(foc->commandBeta / foc->Uref) * radian2Angle;
            if (tempAngle > 0 && tempAngle < 90) // 测试的时候算出了270°，加个if跳过这种情况
                foc->UrefAngle = tempAngle + 90;
        }
        else if (foc->commandBeta < 0) // 3象限
        {
            foc->UrefAngle = asin(foc->commandAlpha / foc->Uref) * radian2Angle + 270;
        }
        else
        {                              // X轴
            if (foc->commandAlpha > 0) // 正半轴
            {
                foc->UrefAngle = 0;
            }
            else if (foc->commandAlpha < 0) // 负半轴
            {
                foc->UrefAngle = 180;
            }
        }
        // ips114_showfloat(0, 2, foc->UrefAngle, 3, 3);
    }
    else
    {                             // Y轴
        if (foc->commandBeta > 0) // 正半轴
        {
            foc->UrefAngle = 90;
        }
        else if (foc->commandBeta < 0) // 负半轴
        {
            foc->UrefAngle = 270;
        }
    }
    UrefAngleTemp = foc->UrefAngle;
    foc->UrefAngle = foc->UrefAngle * foc->cycleGain;
    foc->UrefAngle = fmod(foc->UrefAngle, 360);
    // tempScope(foc->Uref, foc->UrefAngle, *foc->angle, 360);
    // ips114_showfloat(0, 0, foc->Uref, 3, 3);
    // ips114_showfloat(0, 0, *foc->angle, 3, 3);
    // ips114_showfloat(0, 1, foc->UrefAngle, 3, 3);
}
/*------------------------------*/
/*		    变换函数     	     */
/*==============================*/
static void transform(struct Foc *foc)
{
    static int16_t count = 0;
    foc->encoder->read(); // 读取编码器位置信息

    // foc->encoder->absAngle = (int)(foc->encoder->absAngle); // 丢弃精度 | 消抖
    float radian = foc->encoder->absAngle * angle2Radian; // 转子角度转弧度

    count++;
    if (count > 20)
    {
        count = 0;
        foc->sensor->sampling(foc->sensor);               // 电流电压采样
        foc->currentA = -*foc->currentB - *foc->currentC; // 计算A相电流
        clarkTransform(foc);                              // Clark变换
        parkTransform(foc, radian);                       // Park变换

        position->setPoint = foc->targetAngle;
        // augmentedPIDTypeC(position);
        // currentLoopQ->setPoint = position->controllerOutput;

        if (fabs(currentLoopQ->setPoint - foc->Iq) > 0.001) // 误差极小时不计算，防止误差累积
        {
            augmentedPIDTypeC(currentLoopD);
            augmentedPIDTypeC(currentLoopQ);
        }
    }

    invertedParkTransform(foc, radian); // 反Park变换
    angleCalculate(foc);
}

static void calculateSVPWM(struct Foc *foc)
{
    int16_t dutyA = 0, dutyB = 0, dutyC = 0;        // MSB: A
    if (foc->UrefAngle >= 0 && foc->UrefAngle < 60) // I
    {
        float thetaBehind = foc->UrefAngle * angle2Radian;
        float thetaFront = (pi / 3) - thetaBehind;
        int16_t T4 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaFront) * PERIODTIME;  // 100 | A
        int16_t T6 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaBehind) * PERIODTIME; // 110 | A B
        int16_t T7 = (PERIODTIME - T4 - T6) / 2;                                            // 111 | A B C

        dutyA = T4 + T6 + T7;
        dutyB = T6 + T7;
        dutyC = T7;
    }
    else if (foc->UrefAngle < 120) // II
    {
        float thetaBehind = (foc->UrefAngle - 60) * angle2Radian;
        float thetaFront = (pi / 3) - thetaBehind;
        int16_t T6 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaFront) * PERIODTIME;  // 110 | A B
        int16_t T2 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaBehind) * PERIODTIME; // 010 |   B
        int16_t T7 = (PERIODTIME - T6 - T2) / 2;                                            // 111 | A B C

        dutyA = T6 + T7;
        dutyB = T6 + T2 + T7;
        dutyC = T7;
    }
    else if (foc->UrefAngle < 180) // III
    {
        float thetaBehind = (foc->UrefAngle - 120) * angle2Radian;
        float thetaFront = (pi / 3) - thetaBehind;
        int16_t T2 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaFront) * PERIODTIME;  // 010 |   B
        int16_t T3 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaBehind) * PERIODTIME; // 011 |   B C
        int16_t T7 = (PERIODTIME - T2 - T3) / 2;                                            // 111 | A B C

        dutyA = T7;
        dutyB = T2 + T3 + T7;
        dutyC = T3 + T7;
    }
    else if (foc->UrefAngle < 240) // IV
    {
        float thetaBehind = (foc->UrefAngle - 180) * angle2Radian;
        float thetaFront = (pi / 3) - thetaBehind;
        int16_t T3 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaFront) * PERIODTIME;  // 011 |   B C
        int16_t T1 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaBehind) * PERIODTIME; // 001 |     C
        int16_t T7 = (PERIODTIME - T3 - T1) / 2;                                            // 111 | A B C

        dutyA = T7;
        dutyB = T3 + T7;
        dutyC = T3 + T1 + T7;
    }
    else if (foc->UrefAngle < 300) // V
    {
        float thetaBehind = (foc->UrefAngle - 240) * angle2Radian;
        float thetaFront = (pi / 3) - thetaBehind;
        int16_t T1 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaFront) * PERIODTIME;  // 001 |     C
        int16_t T5 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaBehind) * PERIODTIME; // 101 | A   C
        int16_t T7 = (PERIODTIME - T1 - T5) / 2;                                            // 111 | A B C

        dutyA = T5 + T7;
        dutyB = T7;
        dutyC = T1 + T5 + T7;
    }
    else if (foc->UrefAngle < 360) // VI
    {
        float thetaBehind = (foc->UrefAngle - 300) * angle2Radian;
        float thetaFront = (pi / 3) - thetaBehind;
        int16_t T5 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaFront) * PERIODTIME;  // 101 | A   C
        int16_t T4 = (foc->Uref / *foc->voltage) * sqrtof3 * sin(thetaBehind) * PERIODTIME; // 100 | A
        int16_t T7 = (PERIODTIME - T5 - T4) / 2;                                            // 111 | A B C

        dutyA = T5 + T4 + T7;
        dutyB = T7;
        dutyC = T5 + T7;
    }
    dutyUpdate(INH_A, dutyA);
    dutyUpdate(INH_B, dutyB);
    dutyUpdate(INH_C, dutyC);
}
/*------------------------------*/
/*		    临时示波器   	     */
/*==============================*/
static void tempScope(float data1, float data2, float data3, const float max)
{
    const static int16_t length = 240;
    const static int16_t height = 135 / 2;
    static int16_t pos = 0;

    pos++;
    if (pos >= length)
    {
        ips114_clear(WHITE);
        pos = 0;
        // foc.cycleGain = ((int)foc.cycleGain + 1) % 12;
        // ips114_showuint16(0, 7, foc.cycleGain);
    }

    data1 = (data1 / max) * height;
    data2 = (data2 / max) * height;
    data3 = (data3 / max) * height;

    ips114_drawpoint(pos, (int)data1 + height, RED);
    ips114_drawpoint(pos, (int)data2 + height, BLUE);
    ips114_drawpoint(pos, (int)data3 + height, GREEN);
}