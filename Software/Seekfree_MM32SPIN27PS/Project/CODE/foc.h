#ifndef _FOC_H
#define _FOC_H
#include "zf_adc.h"
#include "stdint.h"
#include "magenc.h"
#include "zf_gpio.h"
/*------------------------------------------------------*/
/* 					  枚举类型定义 						*/
/*======================================================*/
typedef enum
{
    ADC_N = ADC_1,       // ADC模块
    ADC_RES = ADC_12BIT, // ADC分辨率

    SO1_CH = ADC1_CH10_B02, // SO1的ADC引脚
    SO2_CH = ADC1_CH09_B01, // SO2的ADC引脚
    VOL_CH = ADC1_CH08_B00, // 电压ADC引脚

    DC_CAL = D3,
    EN_GATE = A12,
} BLDC_ADC_Enum;
/*------------------------------------------------------*/
/* 					    外部声明 						*/
/*======================================================*/
void vacSensorInit(void);
void vacSensorRead(void);

#endif
