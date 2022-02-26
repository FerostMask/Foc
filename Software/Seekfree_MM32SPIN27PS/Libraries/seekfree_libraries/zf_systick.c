/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				systick
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_systick.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief		systick延时函数
// @param		time			需要延时的时间
// @return		void
// Sample usage:				无需用户调用，用户请使用h文件中的宏定义
//-------------------------------------------------------------------------------------------------------------------
void systick_delay (uint32 time)
{
	SysTick->CTRL	= 0x00;
	SysTick->LOAD	= time-1;
	SysTick->VAL	= 0x00;
	SysTick->CTRL	=	SysTick_CTRL_CLKSOURCE_Msk |					//时钟源选择 (core clk)
						SysTick_CTRL_ENABLE_Msk;						//使能 systick
	while( !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		毫秒级systick延时函数
// @param		time			延时多少毫秒
// @return		void
// Sample usage:				systick_delay_ms(1000);   //延时1000毫秒
//-------------------------------------------------------------------------------------------------------------------
void systick_delay_ms (uint32 time)
{
	while(time--) systick_delay(SystemCoreClock / 1000);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		systick定时器
// @param		time			定时时间(0-0x00ffffff)
// @return		void
// Sample usage:				无需用户调用，用户请使用h文件中的宏定义
//-------------------------------------------------------------------------------------------------------------------
void systick_timing(uint32 time)
{
	SysTick->CTRL	= 0x00;
	SysTick->LOAD = time-1;												// 设置延时时间
	SysTick->VAL = 0x00;												// 清空计数器
	NVIC_SetPriority(SysTick_IRQn, 0x0);								// 设置优先级
	SysTick->CTRL = ( 0 
		| SysTick_CTRL_ENABLE_Msk										// 使能 systick
		| SysTick_CTRL_TICKINT_Msk										// 使能中断
		| SysTick_CTRL_CLKSOURCE_Msk									// 时钟源选择 (core clk)
		);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		systick定时器启动
// @param		void
// @return		void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void systick_start(void)
{
	SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;							// 设置延时时间
	SysTick->VAL = 0x00;												// 清空计数器
	SysTick->CTRL = ( 0
		| SysTick_CTRL_ENABLE_Msk										// 使能 systick
		| SysTick_CTRL_CLKSOURCE_Msk									// 时钟源选择 (core clk)
		);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		获得当前System tick timer的值
// @return		返回当前System tick timer的值
// Sample usage:				uint32 tim = systick_getval();
//-------------------------------------------------------------------------------------------------------------------
uint32 systick_getval(void)
{
	return (SysTick_LOAD_RELOAD_Msk - SysTick->VAL);
}

