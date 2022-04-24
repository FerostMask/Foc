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
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
Foc foc = {
    .init = focInit,
};
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
    foc->transform = transform;
    // foc->svpwmAction = calculateSVPWM;

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
    foc->targetAngle = 10;

    // 计算常量初始化
    pi = acos(-1.f);
    angle2Radian = pi / 180;
    radian2Angle = 180 / pi;
    valueOfSin = sin(pi / 3); // sin(2*pi/3) = sin(pi/3)
    sqrtof3 = sqrt(3);

    //? push data into scope
    // scopePushValue(foc->angle, sizeof(*foc->angle), "angle", false);

//    scopePushValue(&foc->currentA, sizeof(foc->currentA), "currentA", false);
//    scopePushValue(foc->currentB, sizeof(*foc->currentB), "currentB", false);
//    scopePushValue(foc->currentC, sizeof(*foc->currentC), "currentC", false);

//    scopePushValue(&foc->alpha, sizeof(foc->alpha), "Alpha", false);
//    scopePushValue(&foc->beta, sizeof(foc->beta), "Beta", false);

    scopePushValue(&foc->Id, sizeof(foc->alpha), "Id", false);
    scopePushValue(&foc->Iq, sizeof(foc->beta), "Iq", false);
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

    //	foc->Iq = -foc->Iq;
    // foc->Id = -foc->Id;

    //    tempScope(foc->Id, foc->Iq, 0, 2);
}
/*------------------------------*/
/*		    变换函数     	     */
/*==============================*/
static void transform(struct Foc *foc)
{
    foc->sensor->sampling(foc->sensor);               // 电流电压采样
    foc->encoder->read();                             // 读取编码器位置信息
    foc->currentA = -*foc->currentB - *foc->currentC; // 计算A相电流

    float radian = foc->encoder->absAngle * angle2Radian; // 转子角度转弧度

    clarkTransform(foc);        // Clark变换
    parkTransform(foc, radian); // Park变换
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