#ifndef _FUNCINTER_H
#define _FUNCINTER_H
/*--------------------------------------------------------------*/
/* 					       枚举类型定义 						*/
/*==============================================================*/
typedef enum
{
    HIGH_LEVEL = 1, // 高电平
    LOW_LEVEL = 0,  // 低电平

    DEFAULT_LEVEL = HIGH_LEVEL, // 初始电平
} DEFAULT_Enum;
/*--------------------------------------------------------------*/
/* 							宏定义函数 							*/
/*==============================================================*/
/*------------------------------*/
/*		        ADC     		*/
/*==============================*/
#define adcInit(pin) adc_init(ADC_N, pin, ADC_RES) // ADC初始化
#define adcRead(pin) adc_convert(ADC_N, pin)       // ADC读取值
/*------------------------------*/
/*		        GPIO     		*/
/*==============================*/
// 初始化
#define outputPinInit(pin) gpio_init(pin, GPO, DEFAULT_LEVEL, GPO_PUSH_PULL)  // 输出引脚初始化 | 推挽输出
#define inputPinInit(pin) gpio_init(pin, GPI, DEFAULT_LEVEL, GPI_FLOATING_IN) // 输入引脚初始化 | 浮空输入
// 引脚操作
#define gpioSetLevel(pin, level) gpio_set(pin, level) // 设置电平
#define gpioSetLow(pin) gpio_set(pin, LOW_LEVEL)      // 拉低电平
#define gpioSetHigh(pin) gpio_set(pin, HIGH_LEVEL)    // 拉高电平
#define gpioReadLevel(pin) gpio_get(pin)              // 读取IO口电平
/*------------------------------*/
/*		       DELAY     		*/
/*==============================*/
#define delay_ns(time) systick_delay_ns(time) // ns级延时
#endif
