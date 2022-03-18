#ifndef _FOC_H
#define _FOC_H

#include "stdint.h"
#include "zf_adc.h"
#include "zf_gpio.h"
#include "zf_pwm.h"
#include "slave.h"
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
} BLDC_ADC_Enum;

typedef enum
{
    TIM_N = TIM_1,         // 定时器模块
    INH_A = TIM_1_CH2_A01, // A相 | 高侧输入
    INL_A = A2,            // A相 | 低侧输入
    INH_B = TIM_1_CH3_A03, // B相 | 高侧输入
    INL_B = A4,            // B相 | 低侧输入
    INH_C = TIM_1_CH1_A08, // C相 | 高侧输入
    INL_C = A7,            // C相 | 低侧输入

    AFMODE_AL = GPIO_AF3, // A相 | 引脚复用模式
    AFMODE_BL = GPIO_AF3, // B相 | 引脚复用模式
    AFMODE_CL = GPIO_AF2, // C相 | 引脚复用模式

    IO_AF_MODE = GPO_AF_PUSH_PUL, // 复用IO模式

    PWM_MAX_DUTY = PWM_DUTY_MAX, // 最大占空比
} BLDC_PWM_Enum;
/*------------------------------------------------------*/
/* 					    外部声明 						*/
/*======================================================*/
void vacSensorInit(void);
void vacSensorRead(void);
void driverInit(void);

#endif
