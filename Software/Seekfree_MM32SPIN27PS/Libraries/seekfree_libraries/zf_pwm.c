/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				pwm
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_pwm.h"
#include "zf_gpio.h"

extern TIM_TypeDef *tim_index[7];
uint32 pwm_enable_state[7];

//-------------------------------------------------------------------------------------------------------------------
// @brief		PWM 引脚初始化 内部调用
// @param		pin				选择 PWM 引脚
// @return		void			NULL
// Sample usage:				pwm_pin_init(pin);
//-------------------------------------------------------------------------------------------------------------------
static void pwm_pin_init (TIM_PWMPIN_enum pin)
{
	afio_init((PIN_enum)(pin &0xff), GPO, (GPIOAF_enum)((pin &0xf00)>>8), GPO_AF_PUSH_PUL);		// 提取对应IO索引 AF功能编码
}

void pwm_init_self(){
	pwm_init(TIM_1, TIM_1_CH1_A08, 10000, 10000);//C | H
	pwm_init(TIM_1, TIM_1_CH2_A01, 10000, 20000);//A | H
	pwm_init(TIM_1, TIM_1_CH3_A03, 10000, 30000);//B | H
	afio_init(A7, GPO, GPIO_AF2, GPO_AF_PUSH_PUL);//CN | L
	afio_init(A2, GPO, GPIO_AF3, GPO_AF_PUSH_PUL);//AN | L
	afio_init(A4, GPO, GPIO_AF3, GPO_AF_PUSH_PUL);//BM | L
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		PWM 初始化
// @param		tim				选择 PWM 使用的 TIM
// @param		pin				选择 PWM 引脚
// @param		freq			设置频率 同个模块只有最后一次设置生效
// @param		duty			设置占空比
// @return		void
// Sample usage:						pwm_init(TIM_1, TIM_1_CH1_A08, 10000, 50000/100*ch1);
//-------------------------------------------------------------------------------------------------------------------
void pwm_init (TIM_enum tim, TIM_PWMPIN_enum pin, uint32 freq, uint32 duty)
{
	if(duty > PWM_DUTY_MAX)	return;																// 占空比写入错误
	uint16 freq_div = ((SystemCoreClock / freq) >> 15);											// 计算预分频
	uint16 period_temp = (SystemCoreClock / freq / (freq_div+1));								// 计算自动重装载值
	uint16 match_temp = (uint16)(period_temp*((float)duty/PWM_DUTY_MAX));						// 计算占空比

	pwm_pin_init(pin);																			// 初始化引脚
	if(tim & 0xf000)
		RCC->APB2ENR |= ((uint32_t)0x00000001 << ((tim&0x0ff0) >> 4));							// 使能时钟
	else
		RCC->APB1ENR |= ((uint32_t)0x00000001 << ((tim&0x0ff0) >> 4));							// 使能时钟

	tim_index[(tim&0x0f)]->ARR = period_temp;													// 装载自动重装载值
	tim_index[(tim&0x0f)]->PSC = freq_div;														// 装载预分频
	tim_index[(tim&0x0f)]->CR1 = TIM_CR1_ARPEN | TIM_CR1_CMS_CENTERALIGNED3;													// 允许自动重装载值的预装载
	tim_index[(tim&0x0f)]->BDTR = TIM_BDTR_MOEN;												// PWM 输出使能

	switch(pin&0xf000)
	{
		case 0x1000:
			tim_index[(tim&0x0f)]->CCMR1 |=														// OC1M [6:4] 110
					TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_2;										// PWM 模式 1
			tim_index[(tim&0x0f)]->CCMR1 |= TIM_CCMR1_OC1PEN;									// 允许输出比较值的预装载
			tim_index[(tim&0x0f)]->CCER |= TIM_CCER_CC1EN | TIM_CCER_CC1NEN;										// 使能通道 1
			tim_index[(tim&0x0f)]->CCR1 = match_temp;											// 装载比较值
			break;
		case 0x2000:
			tim_index[(tim&0x0f)]->CCMR1 |=														// OC1M [6:4] 110
					TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_2;										// PWM 模式 1
			tim_index[(tim&0x0f)]->CCMR1 |= TIM_CCMR1_OC2PEN;									// 允许输出比较值的预装载
			tim_index[(tim&0x0f)]->CCER |= TIM_CCER_CC2EN | TIM_CCER_CC2NEN;										// 使能通道 2
			tim_index[(tim&0x0f)]->CCR2 = match_temp;											// 装载比较值
			break;
		case 0x3000:
			tim_index[(tim&0x0f)]->CCMR2 |=														// OC1M [6:4] 110
					TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_2;										// PWM 模式 1
			tim_index[(tim&0x0f)]->CCMR2 |= TIM_CCMR2_OC3PEN;									// 允许输出比较值的预装载
			tim_index[(tim&0x0f)]->CCER |= TIM_CCER_CC3EN | TIM_CCER_CC3NEN;										// 使能通道 2
			tim_index[(tim&0x0f)]->CCR3 = match_temp;											// 装载比较值
			break;
		case 0x4000:
			tim_index[(tim&0x0f)]->CCMR2 |=														// OC1M [6:4] 110
					TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_2;										// PWM 模式 0
			tim_index[(tim&0x0f)]->CCMR2 |= TIM_CCMR2_OC4PEN;									// 允许输出比较值的预装载
			tim_index[(tim&0x0f)]->CCER |= TIM_CCER_CC4EN;										// 使能通道 2
			tim_index[(tim&0x0f)]->CCR4 = match_temp;											// 装载比较值
			break;
	}
	pwm_enable_state[(tim&0x0f)] = tim_index[(tim&0x0f)]->CCER;

	tim_index[(tim&0x0f)]->CR1 |= TIM_CR1_CEN;													// 使能定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		PWM 使能
// @param		tim				选择 PWM 使用的 TIM
// @return		void
// Sample usage:				pwm_enable(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
void pwm_enable (TIM_enum tim)
{
	tim_index[(tim&0x0f)]->CCER = pwm_enable_state[(tim&0x0f)];
	tim_index[(tim&0x0f)]->CR1 |= TIM_CR1_CEN;													// 使能定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		PWM 失能
// @param		tim				选择 PWM 使用的 TIM
// @return		void
// Sample usage:				pwm_disable(TIM_1);
//-------------------------------------------------------------------------------------------------------------------
void pwm_disable (TIM_enum tim)
{
	pwm_enable_state[(tim&0x0f)] = tim_index[(tim&0x0f)]->CCER;
	tim_index[(tim&0x0f)]->CCER = 0x00000000;
	tim_index[(tim&0x0f)]->CR1 &= ~TIM_CR1_CEN;													// 失能定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		PWM 更新占空比
// @param		tim				选择 PWM 使用的 TIM
// @param		pin				选择 PWM 引脚
// @param		duty			设置占空比
// @return		void
// Sample usage:				pwm_duty_updata(TIM_1, TIM_1_CH1_A08, 500*duty);
//-------------------------------------------------------------------------------------------------------------------
void pwm_duty_updata (TIM_enum tim, TIM_PWMPIN_enum pin, uint32 duty)
{
	if(duty > PWM_DUTY_MAX)	return;																// 占空比写入错误
	uint16 period_temp = tim_index[(tim&0x0f)]->ARR;											// 获取自动重装载值
	uint16 match_temp = (uint16)(period_temp*((float)duty/PWM_DUTY_MAX));						// 计算占空比

	switch(pin&0xf000)																			// 提取通道值
	{
		case 0x1000:
			tim_index[(tim&0x0f)]->CCR1 = match_temp;											// 装载比较值
			break;
		case 0x2000:
			tim_index[(tim&0x0f)]->CCR2 = match_temp;											// 装载比较值
			break;
		case 0x3000:
			tim_index[(tim&0x0f)]->CCR3 = match_temp;											// 装载比较值
			break;
		case 0x4000:
			tim_index[(tim&0x0f)]->CCR4 = match_temp;											// 装载比较值
			break;
	}
}
