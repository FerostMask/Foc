/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				tim
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_tim.h"
#include "zf_gpio.h"

TIM_TypeDef *tim_index[7] = {TIM1, TIM8, TIM2, TIM3, TIM14, TIM16, TIM17};

//-------------------------------------------------------------------------------------------------------------------
// @brief		encoder 引脚初始化 内部调用
// @param		pin				选择 encoder 引脚
// @return		void			NULL
// Sample usage:				tim_encoder_pin_init(pin);
//-------------------------------------------------------------------------------------------------------------------
static void tim_encoder_pin_init (TIM_ENC_PIN_enum pin)
{
	afio_init((PIN_enum)(pin &0xff), GPI, (GPIOAF_enum)((pin &0xf00)>>8), GPI_PULL_UP);			// 提取对应IO索引 AF功能编码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		encoder 初始化
// @param		tim				选择 encoder 使用的 TIM
// @param		PULSEA_pin		选择 encoder 引脚
// @param		PULSEB_pin		选择 encoder 引脚
// @return		void
// Sample usage:				tim_encoder_init(TIM_1, PULSEA_pin, TIM_ENC_PIN_enum);
//-------------------------------------------------------------------------------------------------------------------
void tim_encoder_init (TIM_enum tim, TIM_ENC_PIN_enum PULSEA_pin, TIM_ENC_PIN_enum PULSEB_pin)
{
	if((tim&0x0f) > 0x03) return;																// 基础定时器不支持编码器模式
	tim_encoder_pin_init(PULSEA_pin);
	tim_encoder_pin_init(PULSEB_pin);
	if(tim & 0xf000)
		RCC->APB2ENR |= ((uint32_t)0x00000001 << ((tim&0x0ff0) >> 4));							// 使能时钟
	else
		RCC->APB1ENR |= ((uint32_t)0x00000001 << ((tim&0x0ff0) >> 4));							// 使能时钟

	tim_index[(tim&0x0f)]->ARR = TIM_ENCODER_COUNTER_BASE;										// 装载自动重装载值
	tim_index[(tim&0x0f)]->PSC = 0;																// 装载预分频

	tim_index[(tim&0x0f)]->CCMR1 |= TIM_CCMR1_CC1S_DIRECTTI;									// 输入捕获 IC1 映射到 TI1
	tim_index[(tim&0x0f)]->CCMR1 |= TIM_CCMR1_CC2S_DIRECTTI;									// 输入捕获 IC2 映射到 TI2

	tim_index[(tim&0x0f)]->SMCR |= TIM_SMCR_SMS_ENCODER3;										// 从模式 编码器模式 3 选择

	tim_index[(tim&0x0f)]->CR1 |= TIM_CR1_UDIS;													// 禁止UEV事件的产生
	tim_index[(tim&0x0f)]->CR1 |= TIM_CR1_CEN;													// 使能定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		encoder 复位计数器
// @param		tim				选择 encoder 使用的 TIM
// Sample usage:				tim_encoder_rst(tim);
//-------------------------------------------------------------------------------------------------------------------
void tim_encoder_rst (TIM_enum tim)
{
	tim_index[(tim&0x0f)]->CNT = 0x00;															// 产生更新时间重装载计数器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		读取 encoder 计数器 并复位
// @param		tim				选择 encoder 使用的 TIM
// Sample usage:				temp = tim_encoder_get_count(tim);
//-------------------------------------------------------------------------------------------------------------------
int16 tim_encoder_get_count (TIM_enum tim)
{
	int16 temp = tim_index[(tim&0x0f)]->CNT;
	return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		counter 引脚初始化 内部调用
// @param		pin				选择 counter 引脚
// @return		void			NULL
// Sample usage:				tim_counter_pin_init(pin);
//-------------------------------------------------------------------------------------------------------------------
static void tim_counter_pin_init (TIM_ENC_PIN_enum pin)
{
	afio_init((PIN_enum)(pin &0xff), GPI, (GPIOAF_enum)((pin &0xf00)>>8), GPI_PULL_UP);			// 提取对应IO索引 AF功能编码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		counter 初始化
// @param		tim				选择 counter 使用的 TIM
// @param		input_pin		选择 counter 引脚
// @return		void
// Sample usage:				tim_counter_init(TIM_1, input_pin);
//-------------------------------------------------------------------------------------------------------------------
void tim_counter_init (TIM_enum tim, TIM_ENC_PIN_enum input_pin)
{
	if((tim&0x0f) > 0x03) return;																// 基础定时器不支持从模式
	tim_counter_pin_init(input_pin);

	if(tim & 0xf000)
		RCC->APB2ENR |= ((uint32_t)0x00000001 << ((tim&0x0ff0) >> 4));							// 使能时钟
	else
		RCC->APB1ENR |= ((uint32_t)0x00000001 << ((tim&0x0ff0) >> 4));							// 使能时钟

	tim_index[(tim&0x0f)]->ARR = TIM_ENCODER_COUNTER_BASE;										// 装载自动重装载值
	tim_index[(tim&0x0f)]->PSC = 0;																// 装载预分频

	switch((input_pin&0xf000))
	{
		case 0x1000:
			tim_index[(tim&0x0f)]->CCMR1 |= TIM_CCMR1_CC1S_DIRECTTI;							// 输入捕获 IC1 映射到 TI1
			tim_index[(tim&0x0f)]->SMCR |= TIM_SMCR_TS_TI1FP1;									// 滤波后的定时器输入 1 TI1FP1
			tim_index[(tim&0x0f)]->SMCR |= TIM_SMCR_SMS_EXTERNAL1;								// 从模式 外部时钟1模式 选择
			break;
		case 0x2000:
			tim_index[(tim&0x0f)]->CCMR1 |= TIM_CCMR1_CC2S_DIRECTTI;							// 输入捕获 IC2 映射到 TI2
			tim_index[(tim&0x0f)]->SMCR |= TIM_SMCR_TS_TI2FP2;									// 滤波后的定时器输入 2 TI1FP2
			tim_index[(tim&0x0f)]->SMCR |= TIM_SMCR_SMS_EXTERNAL1;								// 从模式 外部时钟1模式 选择
			break;
	}

	tim_index[(tim&0x0f)]->CR1 |= TIM_CR1_UDIS;													// 禁止UEV事件的产生
	tim_index[(tim&0x0f)]->CR1 |= TIM_CR1_CEN;													// 使能定时器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		counter 复位计数器
// @param		tim				选择 counter 使用的 TIM
// Sample usage:				tim_counter_rst(tim);
//-------------------------------------------------------------------------------------------------------------------
void tim_counter_rst (TIM_enum tim)
{
	tim_index[(tim&0x0f)]->CNT = 0x00;															// 产生更新时间重装载计数器
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		读取 counter 计数器 并复位
// @param		tim				选择 counter 使用的 TIM
// Sample usage:				temp = tim_counter_get_count(tim);
//-------------------------------------------------------------------------------------------------------------------
int16 tim_counter_get_count (TIM_enum tim)
{
	int16 temp = tim_index[(tim&0x0f)]->CNT;
	return temp;
}
