/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				gpio
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_gpio.h"

GPIO_TypeDef *gpio_group[4] = {GPIOA, GPIOB, GPIOC, GPIOD};

//-------------------------------------------------------------------------------------------------------------------
// @brief		GPIO初始化
// @param		pin			选择的引脚 (可选择范围由 common.h 内PIN_enum枚举值确定)
// @param		mode		引脚的方向
// @param		dat			引脚初始化时设置的电平状态，输出时有效 0：低电平 1：高电平
// @param		mode		引脚的模式
// @return		void
// Sample usage:			gpio_init(D5, GPO, 1, GPO_PUSH_PULL);//D5初始化为GPIO功能、输出模式、输出高电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_init (PIN_enum pin, GPIODIR_enum dir, uint8 dat, GPIOMODE_enum mode)
{
	uint32 temp;
	uint8 io_group = ((pin&0xf0)>>4);															// 提取IO分组
	uint8 io_pin = (pin&0x0f);																	// 提取IO引脚下标

	RCC->AHBENR |= RCC_AHBENR_GPIOA << io_group;												// 使能 GPIOx 时钟

	if(io_pin < 0x08)																			// 低8位引脚
	{
		temp = gpio_group[io_group]->CRL;
		temp &= ~(0x0000000f << (io_pin*4));
		temp |= (((uint32)dir|(uint32)mode) << (io_pin*4));
		gpio_group[io_group]->CRL = temp;
	}
	else																						// 高8位引脚
	{
		temp = gpio_group[io_group]->CRH;
		temp &= ~(0x0000000f << ((io_pin-8)*4));
		temp |= (((uint32)dir|(uint32)mode) << ((io_pin-8)*4));
		gpio_group[io_group]->CRH = temp;
	}

	if(dat)
		GPIO_PIN_SET(pin);																		// 初始化电平设置高
	else
		GPIO_PIN_RESET(pin);																	// 初始化电平设置低
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		GPIO初始化
// @param		pin			选择的引脚 (可选择范围由 common.h 内PIN_enum枚举值确定)
// @param		dir			引脚的方向
// @param		af			引脚的功能选择
// @param		mode		引脚的模式
// @return		void
// Sample usage:			afio_init(D5, GPO, GPIO_AF0, GPO_AF_PUSH_PUL);
//-------------------------------------------------------------------------------------------------------------------
void afio_init (PIN_enum pin, GPIODIR_enum dir, GPIOAF_enum af, GPIOMODE_enum mode)
{
	uint32 temp;
	uint8 io_group = ((pin&0xf0)>>4);															// 提取IO分组
	uint8 io_pin = (pin&0x0f);																	// 提取IO引脚下标

	RCC->AHBENR |= RCC_AHBENR_GPIOA << io_group;												// 使能 GPIOx 时钟

	if(io_pin < 0x08)																			// 低8位引脚
	{
		temp = gpio_group[io_group]->AFRL;
		temp &= ~(0x0000000f << (io_pin*4));
		temp |= ((uint32)af << (io_pin*4));
		gpio_group[io_group]->AFRL = temp;

		temp = gpio_group[io_group]->CRL;
		temp &= ~(0x0000000f << (io_pin*4));
		temp |= (((uint32)dir|(uint32)mode) << (io_pin*4));
		gpio_group[io_group]->CRL = temp;
	}
	else																						// 高8位引脚
	{
		temp = gpio_group[io_group]->AFRH;
		temp &= ~(0x0000000f << ((io_pin-8)*4));
		temp |= ((uint32)af << ((io_pin-8)*4));
		gpio_group[io_group]->AFRH = temp;

		temp = gpio_group[io_group]->CRH;
		temp &= ~(0x0000000f << ((io_pin-8)*4));
		temp |= (((uint32)dir|(uint32)mode) << ((io_pin-8)*4));
		gpio_group[io_group]->CRH = temp;
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		GPIO输出设置
// @param		pin			选择的引脚 (可选择范围由 common.h 内PIN_enum枚举值确定)
// @param		dat			0：低电平 1：高电平
// @return		void
// Sample usage:			gpio_set(D5,1);//D5 输出高电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_set (PIN_enum pin, uint8 dat)
{
	if(dat)
		gpio_group[(pin&0xf0)>>4]->BSRR |= (((uint16_t)0x0001) << (pin&0x0f));					// 输出高电平
	else
		gpio_group[(pin&0xf0)>>4]->BRR |= (((uint16_t)0x0001) << (pin&0x0f));					// 输出低电平
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		GPIO输出设置
// @param		pin			选择的引脚 (可选择范围由 common.h 内PIN_enum枚举值确定)
// @return		uint8		0：低电平 1：高电平
// Sample usage:			uint8 status = gpio_get(D5);//获取D5引脚电平
//-------------------------------------------------------------------------------------------------------------------
uint8 gpio_get (PIN_enum pin)
{
	if(gpio_group[(pin&0xf0)>>4]->IDR & (((uint16_t)0x0001) << (pin&0x0f)))						// 读取引脚值
		return 1;
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		GPIO方向设置
// @param		pin			选择的引脚 (可选择范围由 common.h 内PIN_enum枚举值确定)
// @param		dir			引脚的方向   输出：GPO   输入：GPI
// @return		void
// Sample usage:			gpio_dir(D5,GPO);//设置D5为输出模式
//-------------------------------------------------------------------------------------------------------------------
void gpio_dir (PIN_enum pin, GPIODIR_enum dir, GPIOMODE_enum mode)
{
	uint32 temp;
	uint8 io_group = ((pin&0xf0)>>4);															// 提取IO分组
	uint8 io_pin = (pin&0x0f);																	// 提取IO引脚下标
	if(io_pin < 0x08)																			// 低8位引脚
	{
		temp = gpio_group[io_group]->CRL;
		temp &= ~(0x0000000f << (io_pin*4));
		temp |= (((uint32)dir|(uint32)mode) << (io_pin*4));
		gpio_group[io_group]->CRL = temp;
	}
	else																						// 高8位引脚
	{
		temp = gpio_group[io_group]->CRH;
		temp &= ~(0x0000000f << ((io_pin-8)*4));
		temp |= (((uint32)dir|(uint32)mode) << ((io_pin-8)*4));
		gpio_group[io_group]->CRH = temp;
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		GPIO翻转
// @param		pin			选择的引脚 (可选择范围由 common.h 内PIN_enum枚举值确定)
// @return		void
// Sample usage:			gpio_toggle(D5);//设置D5为输出模式
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle (PIN_enum pin)
{
	if(gpio_group[(pin&0xf0)>>4]->IDR & (((uint16_t)0x0001) << (pin&0x0f)))
		gpio_group[(pin&0xf0)>>4]->BRR |= (((uint16_t)0x0001) << (pin&0x0f));					// 输出低电平
	else
		gpio_group[(pin&0xf0)>>4]->BSRR |= (((uint16_t)0x0001) << (pin&0x0f));					// 输出高电平
}
