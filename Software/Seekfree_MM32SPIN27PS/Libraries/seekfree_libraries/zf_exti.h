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

#ifndef _zf_exti_h
#define _zf_exti_h

#include "common.h"
#include "hal_exti.h"
#include "hal_misc.h"
#include "hal_syscfg.h"

void exti_interrupt_init (PIN_enum pin, EXTITrigger_TypeDef trigger, uint8 priority);
void exti_even_init (PIN_enum pin, EXTITrigger_TypeDef trigger);
void exti_interrupt_enable (PIN_enum pin);
void exti_interrupt_disable (PIN_enum pin);
void exti_even_enable (PIN_enum pin);
void exti_even_disable (PIN_enum pin);

#endif
