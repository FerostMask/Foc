/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "math.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "zf_uart.h"
#include "stdio.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder);
static void transform(struct Foc *foc);
static void tempScope(float data1, float data2, float data3, const float max);

float pi;
float angle2Radian;
float valueOfSin;
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
Foc foc = {
    .init = focInit,
};
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

    pi = acos(-1.f);
    angle2Radian = pi / 180;
    valueOfSin = sin(pi / 3); // sin(2*pi/3) = sin(pi/3)
}
/*------------------------------*/
/*		    电流变换     	     */
/*==============================*/
static inline void clarkeTransform(struct Foc *foc)
{
    foc->alpha = foc->currentA - (*foc->currentB / 2) - (*foc->currentC / 2); // cos(2*pi/3) = -0.5 | no need to consider direction
    foc->beta = valueOfSin * (*foc->currentB - *foc->currentC);

    // tempScope(foc->currentA, *foc->currentB, *foc->currentC, 2);
    // tempScope(foc->alpha, foc->beta, 0, 2);

    //	ips114_showfloat(0, 0, foc->alpha, 3, 3);
    //	ips114_showfloat(0, 1, foc->beta, 3, 3);
}

static inline void parkTransform(struct Foc *foc, float radian)
{
    foc->Id = foc->alpha * cos(foc->cycleGain * radian) + foc->beta * sin(foc->cycleGain * radian);
    foc->Iq = -foc->alpha * sin(foc->cycleGain * radian) + foc->beta * cos(foc->cycleGain * radian);

    tempScope(foc->Id, foc->Iq, 0, 2);

    //    ips114_showfloat(0, 0, foc->Id, 3, 3);
    //    ips114_showfloat(0, 1, foc->Iq, 3, 3);
}

static void transform(struct Foc *foc)
{
    foc->sensor->sampling(foc->sensor);               // 电流电压采样
    foc->encoder->read();                             // 读取编码器位置信息
    foc->currentA = -*foc->currentB - *foc->currentC; // 计算A相电流

    float radian = foc->encoder->absAngle * angle2Radian; // 转子角度转弧度

    clarkeTransform(foc);
    parkTransform(foc, radian);
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
