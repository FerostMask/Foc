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

float pi;
float angle2Radian;
float valueOfCos;
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
    valueOfCos = cos(2 * pi / 3);
    valueOfSin = sin(2 * pi / 3);
}

static void clarkeTransform(struct Foc *foc)
{
    foc->sensor->sampling(foc->sensor);                 // 电流电压采样
    foc->encoder->read();                               // 读取编码器位置信息
    foc->currentA = -(*foc->currentB + *foc->currentB); // 计算A相电流

    foc->alpha = foc->currentA - valueOfCos * (*foc->currentB) - valueOfCos * (*foc->currentC);
    foc->beta = valueOfSin * (*foc->currentB) - valueOfSin * (*foc->currentC);

    //	ips114_showfloat(0, 0, foc->alpha, 3, 3);
    //	ips114_showfloat(0, 1, foc->beta, 3, 3);
}

static void parkTransform(struct Foc *foc)
{
    float radian = foc->encoder->absAngle * angle2Radian;
    foc->Id = foc->alpha * cos(foc->cycleGain * radian) + foc->beta * sin(foc->cycleGain * radian);
    foc->Iq = -foc->alpha * sin(foc->cycleGain * radian) + foc->beta * cos(foc->cycleGain * radian);
	
//	char send[30];
//	sprintf(send, "%.2f, %.2f",foc->Id, foc->Iq);
//	
//	uart_putstr(UART_2, send);

//    ips114_showfloat(0, 0, foc->Id, 3, 3);
//    ips114_showfloat(0, 1, foc->Iq, 3, 3);
}

static void transform(struct Foc *foc)
{
    clarkeTransform(foc);
    parkTransform(foc);
}