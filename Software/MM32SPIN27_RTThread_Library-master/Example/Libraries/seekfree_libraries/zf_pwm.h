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

#ifndef _zf_pwm_h
#define _zf_pwm_h

#include "common.h"
#include "hal_tim.h"

#include "zf_tim.h"

#define PWM_DUTY_MAX		50000

typedef enum
{
	// Advanced Timer 16 bits TIM1
	TIM_1_CH1_A08			= 0x1208,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x0008[pin  8]
	TIM_1_CH1_B05			= 0x1615,												// 0x1000[CH1] 0x0060[AF6] 0x0100[B group] 0x0005[pin  5]
	TIM_1_CH1_B07			= 0x1717,												// 0x1000[CH1] 0x0070[AF7] 0x0100[B group] 0x0007[pin  7]
	TIM_1_CH1_B08			= 0x1318,												// 0x1000[CH1] 0x0030[AF3] 0x0100[B group] 0x0008[pin  8]
	TIM_1_CH1_D06			= 0x1536,												// 0x1000[CH1] 0x0050[AF5] 0x0300[D group] 0x0006[pin  6]

	TIM_1_CH2_A01			= 0x2301,												// 0x2000[CH2] 0x0030[AF3] 0x0000[A group] 0x0001[pin  1]
	TIM_1_CH2_A09			= 0x2209,												// 0x2000[CH2] 0x0020[AF2] 0x0000[A group] 0x0009[pin  9]
	TIM_1_CH2_B05			= 0x2715,												// 0x2000[CH2] 0x0070[AF7] 0x0100[B group] 0x0005[pin  5]
	TIM_1_CH2_B06			= 0x2616,												// 0x2000[CH2] 0x0060[AF6] 0x0100[B group] 0x0006[pin  6]

	TIM_1_CH3_A03			= 0x3303,												// 0x3000[CH3] 0x0030[AF3] 0x0000[A group] 0x0003[pin  3]
	TIM_1_CH3_A10			= 0x320A,												// 0x3000[CH3] 0x0020[AF2] 0x0000[A group] 0x000A[pin 10]
	TIM_1_CH3_A11			= 0x330B,												// 0x3000[CH3] 0x0030[AF3] 0x0000[A group] 0x000B[pin 11]
	TIM_1_CH3_B03			= 0x3713,												// 0x3000[CH3] 0x0070[AF7] 0x0100[B group] 0x0003[pin  3]
	TIM_1_CH3_B07			= 0x3617,												// 0x3000[CH3] 0x0060[AF6] 0x0100[B group] 0x0007[pin  7]

	TIM_1_CH4_A11			= 0x420B,												// 0x4000[CH4] 0x0020[AF2] 0x0000[A group] 0x000B[pin 11]

	// Advanced Timer 16 bits TIM8
	TIM_8_CH1_C00			= 0x1620,												// 0x1000[CH1] 0x0060[AF6] 0x0200[C group] 0x0000[pin  0]
	TIM_8_CH1_C06			= 0x1326,												// 0x1000[CH1] 0x0030[AF3] 0x0200[C group] 0x0006[pin  6]

	TIM_8_CH2_C01			= 0x2621,												// 0x2000[CH2] 0x0060[AF6] 0x0200[C group] 0x0001[pin  1]
	TIM_8_CH2_C02			= 0x2522,												// 0x2000[CH2] 0x0050[AF5] 0x0200[C group] 0x0002[pin  2]
	TIM_8_CH2_C07			= 0x2327,												// 0x2000[CH2] 0x0030[AF3] 0x0200[C group] 0x0007[pin  7]

	TIM_8_CH3_C02			= 0x3622,												// 0x3000[CH3] 0x0060[AF6] 0x0200[C group] 0x0002[pin  2]
	TIM_8_CH3_C08			= 0x3328,												// 0x3000[CH3] 0x0030[AF3] 0x0200[C group] 0x0008[pin  8]
	TIM_8_CH3_D04			= 0x3534,												// 0x3000[CH3] 0x0050[AF5] 0x0300[D group] 0x0003[pin  3]

	TIM_8_CH4_C09			= 0x4329,												// 0x4000[CH4] 0x0030[AF3] 0x0200[C group] 0x0009[pin  9]

	// General Timer 32 bits TIM2
	TIM_2_CH1_A00			= 0x1200,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x0000[pin  0]
	TIM_2_CH1_A05			= 0x1205,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x0005[pin  5]
	TIM_2_CH1_A11			= 0x140B,												// 0x1000[CH1] 0x0040[AF4] 0x0000[A group] 0x000B[pin 11]
	TIM_2_CH1_A15			= 0x120F,												// 0x1000[CH1] 0x0020[AF2] 0x0000[A group] 0x000F[pin 15]
	TIM_2_CH1_C07			= 0x1527,												// 0x1000[CH1] 0x0050[AF5] 0x0200[C group] 0x0007[pin  7]
	TIM_2_CH1_C13			= 0x162D,												// 0x1000[CH1] 0x0060[AF6] 0x0200[C group] 0x000D[pin 13]

	TIM_2_CH2_A01			= 0x2201,												// 0x2000[CH2] 0x0020[AF2] 0x0000[A group] 0x0001[pin  1]
	TIM_2_CH2_A12			= 0x240C,												// 0x2000[CH2] 0x0040[AF4] 0x0000[A group] 0x000C[pin 12]
	TIM_2_CH2_B03			= 0x2213,												// 0x2000[CH2] 0x0020[AF2] 0x0100[B group] 0x0003[pin  3]
	TIM_2_CH2_C08			= 0x2528,												// 0x2000[CH2] 0x0050[AF5] 0x0200[C group] 0x0008[pin  8]
	TIM_2_CH2_C14			= 0x262E,												// 0x2000[CH2] 0x0060[AF6] 0x0200[C group] 0x000E[pin 14]

	TIM_2_CH3_A02			= 0x3202,												// 0x3000[CH3] 0x0020[AF2] 0x0000[A group] 0x0002[pin  2]
	TIM_2_CH3_B01			= 0x3411,												// 0x3000[CH3] 0x0040[AF4] 0x0100[B group] 0x0001[pin  1]
	TIM_2_CH3_B10			= 0x321A,												// 0x3000[CH3] 0x0020[AF2] 0x0100[B group] 0x000A[pin 10]
	TIM_2_CH3_C09			= 0x3529,												// 0x3000[CH3] 0x0050[AF5] 0x0200[C group] 0x0009[pin  9]
	TIM_2_CH3_C15			= 0x362F,												// 0x3000[CH3] 0x0060[AF6] 0x0200[C group] 0x000F[pin 15]

	TIM_2_CH4_A03			= 0x4203,												// 0x4000[CH4] 0x0020[AF2] 0x0000[A group] 0x0003[pin  3]
	TIM_2_CH4_B11			= 0x421B,												// 0x4000[CH4] 0x0020[AF2] 0x0100[B group] 0x000B[pin 11]

	// General Timer 16 bits TIM3
	TIM_3_CH1_A06			= 0x1106,												// 0x1000[CH1] 0x0010[AF1] 0x0000[A group] 0x0006[pin  6]
	TIM_3_CH1_B04			= 0x1114,												// 0x1000[CH1] 0x0010[AF1] 0x0100[B group] 0x0004[pin  4]
	TIM_3_CH1_C04			= 0x1524,												// 0x1000[CH1] 0x0050[AF5] 0x0200[C group] 0x0004[pin  4]
	TIM_3_CH1_C06			= 0x1126,												// 0x1000[CH1] 0x0010[AF1] 0x0200[C group] 0x0006[pin  6]
	TIM_3_CH1_D07			= 0x1537,												// 0x1000[CH1] 0x0050[AF5] 0x0300[D group] 0x0007[pin  7]

	TIM_3_CH2_A07			= 0x2107,												// 0x2000[CH2] 0x0010[AF1] 0x0000[A group] 0x0007[pin  7]
	TIM_3_CH2_B05			= 0x2115,												// 0x2000[CH2] 0x0010[AF1] 0x0100[B group] 0x0005[pin  5]
	TIM_3_CH2_B08			= 0x2618,												// 0x2000[CH2] 0x0060[AF6] 0x0100[B group] 0x0008[pin  8]
	TIM_3_CH2_C05			= 0x2525,												// 0x2000[CH2] 0x0050[AF5] 0x0200[C group] 0x0005[pin  5]
	TIM_3_CH2_C07			= 0x2127,												// 0x2000[CH2] 0x0010[AF1] 0x0200[C group] 0x0007[pin  7]

	TIM_3_CH3_B00			= 0x3110,												// 0x3000[CH3] 0x0010[AF1] 0x0100[B group] 0x0000[pin  0]
	TIM_3_CH3_B09			= 0x3619,												// 0x3000[CH3] 0x0060[AF6] 0x0100[B group] 0x0009[pin  9]
	TIM_3_CH3_C06			= 0x3526,												// 0x3000[CH3] 0x0050[AF5] 0x0200[C group] 0x0006[pin  6]
	TIM_3_CH3_C08			= 0x3128,												// 0x3000[CH3] 0x0010[AF1] 0x0200[C group] 0x0008[pin  8]

	TIM_3_CH4_B01			= 0x4111,												// 0x4000[CH4] 0x0010[AF1] 0x0100[B group] 0x0001[pin  1]
	TIM_3_CH4_C09			= 0x4129,												// 0x4000[CH4] 0x0010[AF1] 0x0200[C group] 0x0009[pin  9]

	// Basic Timer 16 bits TIM_14/TIM_16/TIM_17
	TIM_14_CH1_A00			= 0x1600,												// 0x1000[CH1] 0x0060[AF6] 0x0000[A group] 0x0000[pin  0]
	TIM_14_CH1_A04			= 0x1404,												// 0x1000[CH1] 0x0040[AF4] 0x0000[A group] 0x0004[pin  4]
	TIM_14_CH1_A07			= 0x1407,												// 0x1000[CH1] 0x0040[AF4] 0x0000[A group] 0x0007[pin  7]
	TIM_14_CH1_B01			= 0x1011,												// 0x1000[CH1] 0x0000[AF0] 0x0100[B group] 0x0001[pin  1]

	TIM_16_CH1_A06			= 0x1506,												// 0x1000[CH1] 0x0050[AF5] 0x0000[A group] 0x0006[pin  6]
	TIM_16_CH1_B08			= 0x1218,												// 0x1000[CH1] 0x0020[AF2] 0x0100[B group] 0x0008[pin  8]

	TIM_17_CH1_A07			= 0x1507,												// 0x1000[CH1] 0x0050[AF5] 0x0000[A group] 0x0007[pin  7]
	TIM_17_CH1_B09			= 0x1219,												// 0x1000[CH1] 0x0020[AF2] 0x0100[B group] 0x0009[pin  9]
	TIM_17_CH1_B13			= 0x161D,												// 0x1000[CH1] 0x0060[AF6] 0x0100[B group] 0x000D[pin 13]
	TIM_17_CH1_D07			= 0x1637,												// 0x1000[CH1] 0x0060[AF6] 0x0300[D group] 0x0007[pin  7]
}TIM_PWMPIN_enum;

void pwm_init (TIM_enum tim, TIM_PWMPIN_enum pin, uint32 freq, uint32 duty);
void pwm_enable (TIM_enum tim);
void pwm_disable (TIM_enum tim);
void pwm_duty_updata (TIM_enum tim, TIM_PWMPIN_enum pin, uint32 duty);

#endif
