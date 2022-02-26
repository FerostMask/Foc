/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				camera
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#ifndef _zf_camera_h
#define _zf_camera_h

#include "common.h"
#include "hal_dma.h"
#include "hal_rcc.h"

#include "zf_tim.h"

// 枚举 TIM_ETR 输入引脚	此枚举定义不允许用户修改
typedef enum																		// 枚举ADC通道
{
	TIM_1_ETR_A12			= 0x020C,												// 0x0000[TIM1] 0x0020[AF2] 0x0000[A group] 0x000C[pin 12]
	TIM_1_ETR_A15			= 0x020F,												// 0x0000[TIM1] 0x0020[AF2] 0x0000[A group] 0x000F[pin 15]

	TIM_2_ETR_A00			= 0x2200,												// 0x2000[TIM2] 0x0020[AF2] 0x0000[A group] 0x0000[pin  0]
	TIM_2_ETR_A05			= 0x2200,												// 0x2000[TIM2] 0x0020[AF2] 0x0000[A group] 0x0005[pin  5]
	TIM_2_ETR_A11			= 0x240B,												// 0x2000[TIM2] 0x0040[AF4] 0x0000[A group] 0x000B[pin 11]
	TIM_2_ETR_C07			= 0x2527,												// 0x2000[TIM2] 0x0050[AF5] 0x0200[C group] 0x0007[pin  7]
	TIM_2_ETR_C13			= 0x262D,												// 0x2000[TIM2] 0x0060[AF6] 0x0200[C group] 0x000D[pin 13]

	TIM_3_ETR_D06			= 0x3136,												// 0x3000[TIM3] 0x0010[AF1] 0x0300[D group] 0x0006[pin  6]
}TIM_ETR_PIN_enum;

extern CAMERA_TYPE_enum camera_type;

void camera_dma_init(DMA_Channel_TypeDef* dma_ch,uint32 src_addr, uint32 des_addr, uint32 size);
void camera_tim_etr_init(TIM_ETR_PIN_enum pin);

#endif
