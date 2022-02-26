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

#ifndef _zf_tim_h
#define _zf_tim_h

#include "common.h"
#include "hal_tim.h"
#include "hal_misc.h"

#define TIM_ENCODER_COUNTER_BASE			0xFFFF

typedef enum
{
	// Advanced Timer 16 bits
	TIM_1					= 0x10B0,												// 0x1000[APB2]	0x00B0[APB_EN BIT] 0x0000[TIM index]
	TIM_8					= 0x10D1,												// 0x1000[APB2]	0x00D0[APB_EN BIT] 0x0001[TIM index]

	// General Timer 32 bits
	TIM_2					= 0x0002,												// 0x0000[APB1]	0x0000[APB_EN BIT] 0x0002[TIM index]

	// General Timer 16 bits
	TIM_3					= 0x0013,												// 0x0000[APB1]	0x0010[APB_EN BIT] 0x0003[TIM index]

	// Basic Timer 16 bits
	TIM_14					= 0x1104,												// 0x1000[APB2]	0x0100[APB_EN BIT] 0x0004[TIM index]
	TIM_16					= 0x1115,												// 0x1000[APB2]	0x0110[APB_EN BIT] 0x0005[TIM index]
	TIM_17					= 0x1126,												// 0x1000[APB2]	0x0120[APB_EN BIT] 0x0006[TIM index]
}TIM_enum;

typedef enum
{
	// Advanced Timer 16 bits TIM1
	TIM_1_ENC1_A08			= 0x1208,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x0008[pin  8]
	TIM_1_ENC1_B05			= 0x1615,												// 0x1000[CH1] 0x0060[AF6] 0x0100[B group] 0x0005[pin  5]
	TIM_1_ENC1_B07			= 0x1717,												// 0x1000[CH1] 0x0070[AF7] 0x0100[B group] 0x0007[pin  7]
	TIM_1_ENC1_B08			= 0x1318,												// 0x1000[CH1] 0x0030[AF3] 0x0100[B group] 0x0008[pin  8]
	TIM_1_ENC1_D06			= 0x1536,												// 0x1000[CH1] 0x0050[AF5] 0x0300[D group] 0x0006[pin  6]

	TIM_1_ENC2_A01			= 0x2301,												// 0x2000[CH2] 0x0030[AF3] 0x0000[A group] 0x0001[pin  1]
	TIM_1_ENC2_A09			= 0x2209,												// 0x2000[CH2] 0x0020[AF2] 0x0000[A group] 0x0009[pin  9]
	TIM_1_ENC2_B05			= 0x2715,												// 0x2000[CH2] 0x0070[AF7] 0x0100[B group] 0x0005[pin  5]
	TIM_1_ENC2_B06			= 0x2616,												// 0x2000[CH2] 0x0060[AF6] 0x0100[B group] 0x0006[pin  6]

	// Advanced Timer 16 bits TIM8
	TIM_8_ENC1_C00			= 0x1620,												// 0x1000[CH1] 0x0060[AF6] 0x0200[C group] 0x0000[pin  0]
	TIM_8_ENC1_C06			= 0x1326,												// 0x1000[CH1] 0x0030[AF3] 0x0200[C group] 0x0006[pin  6]

	TIM_8_ENC2_C01			= 0x2621,												// 0x2000[CH2] 0x0060[AF6] 0x0200[C group] 0x0001[pin  1]
	TIM_8_ENC2_C02			= 0x2522,												// 0x2000[CH2] 0x0050[AF5] 0x0200[C group] 0x0002[pin  2]
	TIM_8_ENC2_C07			= 0x2327,												// 0x2000[CH2] 0x0030[AF3] 0x0200[C group] 0x0007[pin  7]

	// General Timer 32 bits TIM2
	TIM_2_ENC1_A00			= 0x1200,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x0000[pin  0]
	TIM_2_ENC1_A05			= 0x1205,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x0005[pin  5]
	TIM_2_ENC1_A11			= 0x140B,												// 0x1000[CH1] 0x0040[AF4] 0x0000[A group] 0x000B[pin 11]
	TIM_2_ENC1_A15			= 0x120F,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x000F[pin 15]
	TIM_2_ENC1_C07			= 0x1527,												// 0x1000[CH1] 0x0050[AF5] 0x0200[C group] 0x0007[pin  7]
	TIM_2_ENC1_C13			= 0x162D,												// 0x1000[CH1] 0x0060[AF6] 0x0200[C group] 0x000D[pin 13]

	TIM_2_ENC2_A01			= 0x2201,												// 0x2000[CH2] 0x0020[AF2] 0x0000[A group] 0x0001[pin  1]
	TIM_2_ENC2_A12			= 0x240C,												// 0x2000[CH2] 0x0040[AF4] 0x0000[A group] 0x000C[pin 12]
	TIM_2_ENC2_B03			= 0x2213,												// 0x2000[CH2] 0x0020[AF2] 0x0100[B group] 0x0003[pin  3]
	TIM_2_ENC2_C08			= 0x2528,												// 0x2000[CH2] 0x0050[AF5] 0x0200[C group] 0x0008[pin  8]
	TIM_2_ENC2_C14			= 0x262E,												// 0x2000[CH2] 0x0060[AF6] 0x0200[C group] 0x000E[pin 14]

	// General Timer 16 bits TIM3
	TIM_3_ENC1_A06			= 0x1106,												// 0x1000 [CH1] 0x0010[AF1] 0x0000[A group] 0x0006[pin  6]
	TIM_3_ENC1_B04			= 0x1114,												// 0x1000 [CH1] 0x0010[AF1] 0x0100[B group] 0x0004[pin  4]
	TIM_3_ENC1_C04			= 0x1524,												// 0x1000 [CH1] 0x0050[AF5] 0x0200[C group] 0x0004[pin  4]
	TIM_3_ENC1_C06			= 0x1126,												// 0x1000 [CH1] 0x0010[AF1] 0x0200[C group] 0x0006[pin  6]
	TIM_3_ENC1_D07			= 0x1537,												// 0x1000 [CH1] 0x0050[AF5] 0x0300[D group] 0x0007[pin  7]

	TIM_3_ENC2_A07			= 0x2107,												// 0x2000 [CH2] 0x0010[AF1] 0x0000[A group] 0x0007[pin  7]
	TIM_3_ENC2_B05			= 0x2115,												// 0x2000 [CH2] 0x0010[AF1] 0x0100[B group] 0x0005[pin  5]
	TIM_3_ENC2_B08			= 0x2618,												// 0x2000 [CH2] 0x0060[AF6] 0x0100[B group] 0x0008[pin  8]
	TIM_3_ENC2_C05			= 0x2525,												// 0x2000 [CH2] 0x0050[AF5] 0x0200[C group] 0x0005[pin  5]
	TIM_3_ENC2_C07			= 0x2127,												// 0x2000 [CH2] 0x0010[AF1] 0x0200[C group] 0x0007[pin  7]
}TIM_ENC_PIN_enum;

extern TIM_TypeDef *tim_index[7];

void	tim_encoder_init			(TIM_enum tim, TIM_ENC_PIN_enum PULSEA_pin, TIM_ENC_PIN_enum PULSEB_pin);
void	tim_encoder_rst				(TIM_enum tim);
int16	tim_encoder_get_count		(TIM_enum tim);

void	tim_counter_init			(TIM_enum tim, TIM_ENC_PIN_enum input_pin);
void	tim_counter_rst				(TIM_enum tim);
int16	tim_counter_get_count		(TIM_enum tim);

#endif
