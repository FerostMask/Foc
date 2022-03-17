/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "SEEKFREE_IPS114_SPI.h"
/*--------------------------------------------------------------*/
/* 							宏定义函数 							*/
/*==============================================================*/
#define adcInit(pin) adc_init(ADC_N, pin, ADC_RES) // ADC初始化
#define adcRead(pin) adc_convert(ADC_N, pin)       // ADC读取值

#define gpioSetLow(pin) gpio_set(pin, LOW_LEVEL)                             // 拉低电平
#define gpioSetHigh(pin) gpio_set(pin, HIGH_LEVEL)                           // 拉高电平
#define outputPinInit(pin) gpio_init(pin, GPO, DEFAULT_LEVEL, GPO_PUSH_PULL) // 输出引脚初始化 | 推挽输出
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
    outputPinInit(DC_CAL);
    outputPinInit(EN_GATE);
    gpioSetHigh(DC_CAL);
    gpioSetHigh(EN_GATE);
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
