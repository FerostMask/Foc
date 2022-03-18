/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
void vacSensorInit(void)
{
    adcInit(SO1_CH);
    adcInit(SO2_CH);
    adcInit(VOL_CH);
}

void vacSensorRead(void)
{
    uint16_t current1 = adcRead(SO1_CH);
    uint16_t current2 = adcRead(SO2_CH);
    uint16_t voltage = adcRead(VOL_CH);
    ips114_showuint16(0, 0, current1);
    ips114_showuint16(0, 1, current2);
    ips114_showuint16(0, 2, voltage);
}

void driverInit(void)
{
    pwmInit(INH_A, 10000, 0);   // A | H
    pwmInit(INH_B, 10000, 0);   // B | H
    pwmInit(INH_C, 10000, 0);   // C | H
    afioInit(INL_A, AFMODE_AL); // AN | L
    afioInit(INL_B, AFMODE_BL); // BN | L
    afioInit(INL_C, AFMODE_CL); // CN | L
}