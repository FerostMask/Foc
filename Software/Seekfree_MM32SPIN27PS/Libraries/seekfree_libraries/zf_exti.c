/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				exti
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_exti.h"
#include "zf_gpio.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief		EXTI 中断初始化
// @param		pin				选择 EXTI 引脚
// @param		trigger			选择触发的信号方式 [EXTI_Trigger_Rising/EXTI_Trigger_Falling/EXTI_Trigger_Rising_Falling]
// @param		priority		选择该中断（ EXTI0_1_IRQn/EXTI2_3_IRQn/EXTI4_15_IRQn ）优先级 范围 [0-3]
// @return		void
// Sample usage:				exti_interrupt_init(A0, EXTI_Trigger_Rising, 0x01);
//-------------------------------------------------------------------------------------------------------------------
void exti_interrupt_init (PIN_enum pin, EXTITrigger_TypeDef trigger, uint8 priority)
{
	NVIC_InitTypeDef NVIC_InitStructure;														// 中断配置结构体
	EXTI_InitTypeDef EXTI_InitStructure;														// EXTI 配置结构体

	gpio_init(pin, GPI, GPIO_HIGH, GPI_PULL_UP);													// 初始化选中的引脚

	RCC->APB2ENR |= RCC_APB2ENR_EXTI;
	SYSCFG_EXTILineConfig(((pin&0xf0) >> 4), (pin&0x0f));										// 先启用对应的 GPIO 组别的 EXTI 输入使能
	EXTI_StructInit(&EXTI_InitStructure);														// 获取默认的 EXTI 配置
	EXTI_InitStructure.EXTI_Line = 0x00000001 << (pin&0x0f);									// 设置对应的 LINE
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;											// 设置中断模式
	EXTI_InitStructure.EXTI_Trigger = trigger;													// 设置触发方式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;													// 使能
	EXTI_Init(&EXTI_InitStructure);																// 初始化 EXTI

	if((pin&0x0f) < 2)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;										// EXTI0_1 是一个中断
	else if((pin&0x0f) < 4)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;										// EXTI2_3 是一个中断
	else
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;										// EXTI4_15 是一个中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = priority;										// 设置优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;												// 使能中断
	NVIC_Init(&NVIC_InitStructure);																// 初始化中断配置
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		EXTI 事件初始化
// @param		pin				选择 EXTI 引脚
// @param		trigger			选择触发的信号方式 [EXTI_Trigger_Rising/EXTI_Trigger_Falling/EXTI_Trigger_Rising_Falling]
// @return		void
// Sample usage:				exti_even_init(A0,EXTI_Trigger_Rising);
//-------------------------------------------------------------------------------------------------------------------
void exti_even_init (PIN_enum pin, EXTITrigger_TypeDef trigger)
{
	EXTI_InitTypeDef EXTI_InitStructure;														// EXTI 配置结构体

	gpio_init(pin, GPI, GPIO_LOW, GPI_PULL_UP);													// 初始化选中的引脚

	SYSCFG_EXTILineConfig(((pin&0xf0) >> 4), (pin&0x0f));										// 先启用对应的 GPIO 组别的 EXTI 输入使能
	EXTI_StructInit(&EXTI_InitStructure);														// 获取默认的 EXTI 配置
	EXTI_InitStructure.EXTI_Line = (pin&0x0f);													// 设置对应的 LINE
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;												// 设置事件模式
	EXTI_InitStructure.EXTI_Trigger = trigger;													// 设置触发方式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;													// 使能
	EXTI_Init(&EXTI_InitStructure);																// 初始化 EXTI
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		EXTI 中断使能
// @param		pin				选择 EXTI 引脚
// @return		void
// Sample usage:				exti_interrupt_enable(A0);
//-------------------------------------------------------------------------------------------------------------------
void exti_interrupt_enable (PIN_enum pin)
{
	EXTI->IMR |= (0x00000001 << (pin&0x0f));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		EXTI 中断失能
// @param		pin				选择 EXTI 引脚
// @return		void
// Sample usage:				exti_interrupt_disable(A0);
//-------------------------------------------------------------------------------------------------------------------
void exti_interrupt_disable (PIN_enum pin)
{
	EXTI->IMR &= ~(0x00000001 << (pin&0x0f));
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief		EXTI 事件使能
//  @param		pin				选择 EXTI 引脚
//  @return		void
//  Sample usage:				exti_even_enable(A0);
//-------------------------------------------------------------------------------------------------------------------
void exti_even_enable (PIN_enum pin)
{
	EXTI->EMR |= (0x00000001 << (pin&0x0f));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		EXTI 事件失能
// @param		pin				选择 EXTI 引脚
// @return		void
// Sample usage:				exti_even_disable(A0);
//-------------------------------------------------------------------------------------------------------------------
void exti_even_disable (PIN_enum pin)
{
	EXTI->EMR &= ~(0x00000001 << (pin&0x0f));
}
