/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				pit
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#ifndef _zf_pit_h
#define _zf_pit_h

#include "common.h"
#include "zf_tim.h"

void tim_interrupt_init (TIM_enum tim, uint32 freq, uint8 priority);
void tim_interrupt_init_us (TIM_enum tim, uint32 timer, uint8 priority);
void tim_interrupt_init_ms (TIM_enum tim, uint32 timer, uint8 priority);
void tim_interrupt_enable (TIM_enum tim);
void tim_interrupt_disable (TIM_enum tim);

#endif
