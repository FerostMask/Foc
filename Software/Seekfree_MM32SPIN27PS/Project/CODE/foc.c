/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "math.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "zf_uart.h"
#include "stdio.h"

#define PERIODTIME PWM_MAX_DUTY
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
// Foc
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder);
static void transform(struct Foc *foc);
static void tempScope(float data1, float data2, float data3, const float max);
// PID
void augmentedPID(struct pidpara *para, const short tar);
void positionPID(struct pidpara *para, const short tar);

float pi;
float angle2Radian;
float radian2Angle;
float valueOfSin;
float sqrtof3;
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
Foc foc = {
    .init = focInit,
};

pidpara currentLoopQ = {
    .alpha = 0.3,
    .Kp = 0.8,
    .Ki = 0.8,
    .Kd = 0,
    .thrsod = 0.5,
};

pidpara currentLoopD;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		    Foc初始化    	     */
/*==============================*/
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder)
{
    foc->transform = transform;

    foc->driver = driver;
    foc->sensor = driver->sensor;
    foc->encoder = encoder;

    foc->currentA = 0;
    foc->currentB = &(driver->sensor->current2);
    foc->currentC = &(driver->sensor->current1);

    foc->voltage = &(driver->sensor->voltage);

    foc->angle = &(encoder->absAngle);

    foc->cycleGain = 7;

    foc->targetCurrent = 0;

    // 常量初始化
    pi = acos(-1.f);
    angle2Radian = pi / 180;
    radian2Angle = 180 / pi;
    valueOfSin = sin(pi / 3); // sin(2*pi/3) = sin(pi/3)
    sqrtof3 = sqrt(3);

    // PID初始化
    currentLoopD.alpha = currentLoopQ.alpha;
    currentLoopD.Kp = currentLoopQ.Kp;
    currentLoopD.Ki = currentLoopQ.Ki;
    currentLoopD.Kd = currentLoopQ.Kd;
    currentLoopD.thrsod = currentLoopQ.thrsod;

    currentLoopQ.act = &foc->Iq;
    currentLoopD.act = &foc->Id;
    foc->afterIq = &currentLoopQ.rs;
    foc->afterId = &currentLoopD.rs;
}
/*------------------------------*/
/*		    电流变换     	     */
/*==============================*/
static inline void clarkTransform(struct Foc *foc)
{
    foc->alpha = foc->currentA - (*foc->currentB / 2) - (*foc->currentC / 2); // cos(2*pi/3) = -0.5 | no need to consider direction
    foc->beta = valueOfSin * (*foc->currentB - *foc->currentC);

    // tempScope(foc->currentA, *foc->currentB, *foc->currentC, 2);
    // tempScope(foc->alpha, foc->beta, 0, 2);

    //	ips114_showfloat(0, 0, foc->alpha, 3, 3);
    //	ips114_showfloat(0, 1, foc->beta, 3, 3);
}

static inline void parkTransform(struct Foc *foc, const float radian)
{
    foc->Id = foc->alpha * cos(foc->cycleGain * radian) + foc->beta * sin(foc->cycleGain * radian);
    foc->Iq = -foc->alpha * sin(foc->cycleGain * radian) + foc->beta * cos(foc->cycleGain * radian);

    // tempScope(foc->Id, foc->Iq, 0, 2);

    //    ips114_showfloat(0, 0, foc->Id, 3, 3);
    //    ips114_showfloat(0, 1, foc->Iq, 3, 3);
}

static inline void reverseParkTransform(struct Foc *foc, const float radian)
{
    // foc->revAlpha = *foc->afterId * cos(foc->cycleGain * radian) - *foc->afterIq * sin(foc->cycleGain * radian);
    // foc->revBeta = *foc->afterId * sin(foc->cycleGain * radian) + *foc->afterIq * cos(foc->cycleGain * radian);

    foc->revAlpha = *foc->afterId * cos(radian) - *foc->afterIq * sin(radian);
    foc->revBeta = *foc->afterId * sin(radian) + *foc->afterIq * cos(radian);

    // tempScope(foc->revAlpha, foc->revBeta, 0, 2);
}

static inline void angleCalculate(struct Foc *foc)
{
    foc->Uref = pow((foc->revAlpha * foc->revAlpha) + (foc->revBeta * foc->revBeta), 1.f / 2.f); // 计算Uref
    // if (foc->Uref < 0.001)
    //     return;
    // 计算Uref的角度
    if (foc->revAlpha > 0) // 1、4象限
    {
        foc->UrefAngle = atan(foc->revBeta / foc->revAlpha) * radian2Angle;
        if (foc->UrefAngle < 0)
        {
            foc->UrefAngle += 360;
        }
        // ips114_showfloat(0, 2, foc->UrefAngle, 3, 3);
    }
    else if (foc->revAlpha < 0)
    {
        if (foc->revBeta > 0) // 2象限
        {
            float tempAngle = acos(foc->revBeta / foc->Uref) * radian2Angle;
            if (tempAngle > 0 && tempAngle < 90) // 测试的时候算出了270°，加个if跳过这种情况
                foc->UrefAngle = tempAngle + 90;
        }
        else if (foc->revBeta < 0) // 3象限
        {
            foc->UrefAngle = asin(foc->revAlpha / foc->Uref) * radian2Angle + 270;
        }
        else
        {                          // X轴
            if (foc->revAlpha > 0) // 正半轴
            {
                foc->UrefAngle = 0;
            }
            else if (foc->revAlpha < 0) // 负半轴
            {
                foc->UrefAngle = 180;
            }
        }
        // ips114_showfloat(0, 2, foc->UrefAngle, 3, 3);
    }
    else
    {                         // Y轴
        if (foc->revBeta > 0) // 正半轴
        {
            foc->UrefAngle = 90;
        }
        else if (foc->revBeta < 0) // 负半轴
        {
            foc->UrefAngle = 270;
        }
    }
    tempScope(foc->Uref, foc->UrefAngle, *foc->angle, 360);
    // ips114_showfloat(0, 0, foc->Uref, 3, 3);
    // ips114_showfloat(0, 0, *foc->angle, 3, 3);
    // ips114_showfloat(0, 1, foc->UrefAngle, 3, 3);
}

static inline void calculateSVPWM(struct Foc *foc)
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
        // ips114_showuint16(0, 0, T4);
        // ips114_showuint16(0, 1, T6);
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
        // ips114_showuint16(0, 0, T6);
        // ips114_showuint16(0, 1, T2);
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
        // ips114_showuint16(0, 0, T2);
        // ips114_showuint16(0, 1, T3);
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
        // ips114_showuint16(0, 0, T3);
        // ips114_showuint16(0, 1, T1);
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
        // ips114_showuint16(0, 0, T1);
        // ips114_showuint16(0, 1, T5);
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
        // ips114_showuint16(0, 0, T5);
        // ips114_showuint16(0, 1, T4);
    }
    dutyUpdate(INH_A, dutyA);
    dutyUpdate(INH_B, dutyB);
    dutyUpdate(INH_C, dutyC);
    // ips114_showuint16(0, 0, dutyA);
    // ips114_showuint16(0, 1, dutyB);
    // ips114_showuint16(0, 2, dutyC);
}

static void transform(struct Foc *foc)
{
    foc->sensor->sampling(foc->sensor);               // 电流电压采样
    foc->encoder->read();                             // 读取编码器位置信息
    foc->currentA = -*foc->currentB - *foc->currentC; // 计算A相电流

    float radian = foc->encoder->absAngle * angle2Radian; // 转子角度转弧度

    clarkTransform(foc);        // Clark变换
    parkTransform(foc, radian); // Park变换

    augmentedPID(&currentLoopQ, foc->targetCurrent); // 电流环PID计算
    augmentedPID(&currentLoopD, 0);

    reverseParkTransform(foc, radian); // 反Park变换

    angleCalculate(foc); // 计算Uref的值和角度
    // calculateSVPWM(foc);
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
        //		foc.cycleGain = ((int)foc.cycleGain +1) % 12;
        //		ips114_showuint16(0, 7, foc.cycleGain);
    }

    data1 = (data1 / max) * height;
    data2 = (data2 / max) * height;
    data3 = (data3 / max) * height;

    ips114_drawpoint(pos, (int)data1 + height, RED);
    ips114_drawpoint(pos, (int)data2 + height, BLUE);
    ips114_drawpoint(pos, (int)data3 + height, GREEN);
}

/*----------------------*/
/*		增量PID模块		*/
/*======================*/
void augmentedPID(struct pidpara *para, const short tar)
{
    //	参数列表-> para:调定参数 | tar:目标值 | act:实际值 | value:控制量 | thrsod:阈值
    //	变量定义
    float yn;
    //	保存和计算误差
    para->e3 = para->e2;
    para->e2 = para->e1;
    para->e1 = tar - *para->act;
    yn = para->I;
    //	PID公式
    para->I = para->Ki * para->e1;
    //	一阶低通滤波（积分项
    para->I = para->alpha * para->I + (1 - para->alpha) * yn;
    para->rs += para->Kp * (para->e1 - para->e2) + para->I + para->Kd * (para->e1 - 2 * para->e2 + para->e3);
    //	阈值限定
    if (fabs(para->rs) > para->thrsod)
    {
        if (para->rs >= 0)
            para->rs = para->thrsod;
        else
            para->rs = -para->thrsod;
    }
}
/*----------------------*/
/*		位置PID模块		*/
/*======================*/
void positionPID(struct pidpara *para, const short tar)
{
    //	参数列表-> para:调定参数 | tar:目标值 | act:实际值 | max:最大控制值 | min:最小控制值
    //	保存和计算误差
    para->e2 = para->e1;
    para->e1 = tar - *para->act;
    //	PID公式
    para->rs = (para->Kp) * para->e1 + para->Kd * (para->e1 - para->e2);
    //	阈值限定
    if (fabs(para->rs) > para->thrsod)
    {
        if (para->rs >= 0)
            para->rs = para->thrsod;
        else
            para->rs = -para->thrsod;
    }
}