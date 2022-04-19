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
// "zf_adc.h"
#define adcInit(pin) adc_init(ADC_N, pin, ADC_RES) // ADC初始化
#define adcRead(pin) adc_convert(ADC_N, pin)       // ADC读取值
/*------------------------------*/
/*		        GPIO     		*/
/*==============================*/
// "zf_gpio.h"
// 初始化
#define outputPinInit(pin) gpio_init(pin, GPO, DEFAULT_LEVEL, GPO_PUSH_PULL)  // 输出引脚初始化 | 推挽输出
#define inputPinInit(pin) gpio_init(pin, GPI, DEFAULT_LEVEL, GPI_FLOATING_IN) // 输入引脚初始化 | 浮空输入
// 引脚操作
#define gpioSetLevel(pin, level) gpio_set(pin, level) // 设置电平
#define gpioSetLow(pin) gpio_set(pin, LOW_LEVEL)      // 拉低电平
#define gpioSetHigh(pin) gpio_set(pin, HIGH_LEVEL)    // 拉高电平
#define gpioReadLevel(pin) gpio_get(pin)              // 读取IO口电平
/*------------------------------*/
/*		        GPIO     		*/
/*==============================*/
#define uartInit(baud, tx, rx) uart_init(UART_N, baud, tx, rx) // UART初始化
#define uartPutString(str) uart_putstr(UART_N, str)
#define uartPutBuff(src, len) uart_putbuff(UART_N, src, len)
/*------------------------------*/
/*		       DELAY     		*/
/*==============================*/
// "zf_systick.h"
#define delay_ns(time) systick_delay_ns(time) // ns级延时
#define delay_us(time) systick_delay_us(time) // us级延时
#define delay_ms(time) systick_delay_ms(time) // ms级延时
/*------------------------------*/
/*		        PWM     		*/
/*==============================*/
// "zf_pwm.h"
// "zf_gpio.h"
#define pwmInit(pin, freq, duty) pwm_init(TIM_N, pin, freq, duty)     // PWM引脚初始化 | 中心对齐模式
#define afioInit(pin, afMode) afio_init(pin, GPO, afMode, IO_AF_MODE) // PWM互补输出引脚初始化
#define dutyUpdate(pin, duty) pwm_duty_updata(TIM_N, pin, duty)       // PWM更新占空比
#endif
