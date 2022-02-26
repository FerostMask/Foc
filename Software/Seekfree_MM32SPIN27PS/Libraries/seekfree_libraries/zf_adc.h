/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				adc
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#ifndef _zf_adc_h
#define _zf_adc_h

#include "common.h"

// 枚举 ADC 引脚通道	此枚举定义不允许用户修改
typedef enum																		// 枚举ADC通道
{
	ADC1_CH00_A00		= 0x000,													// 0x000[CH00] 0x000[A group] 0x000[pin  0]
	ADC1_CH01_A01		= 0x101,													// 0x100[CH01] 0x000[A group] 0x001[pin  1]
	ADC1_CH02_A02		= 0x202,													// 0x200[CH02] 0x000[A group] 0x002[pin  2]
	ADC1_CH03_A03		= 0x303,													// 0x300[CH03] 0x000[A group] 0x003[pin  3]
	ADC1_CH04_A04		= 0x404,													// 0x400[CH04] 0x000[A group] 0x004[pin  4]
	ADC1_CH05_A05		= 0x505,													// 0x500[CH05] 0x000[A group] 0x005[pin  5]
	ADC1_CH06_A06		= 0x606,													// 0x600[CH06] 0x000[A group] 0x006[pin  6]
	ADC1_CH07_A07		= 0x707,													// 0x700[CH07] 0x000[A group] 0x007[pin  7]
	ADC1_CH08_B00		= 0x810,													// 0x800[CH08] 0x010[B group] 0x000[pin  0]
	ADC1_CH09_B01		= 0x911,													// 0x900[CH09] 0x010[B group] 0x001[pin  1]
	ADC1_CH10_B02		= 0xA12,													// 0xA00[CH10] 0x010[B group] 0x002[pin  2]
	ADC1_CH11_B10		= 0xB1A,													// 0xB00[CH11] 0x010[B group] 0x00A[pin 10]
	
	ADC2_CH00_A10		= 0x00A,													// 0x000[CH00] 0x000[A group] 0x00A[pin 10]
	ADC2_CH01_B15		= 0x11F,													// 0x100[CH01] 0x010[B group] 0x00F[pin 15]
	ADC2_CH02_B14		= 0x21E,													// 0x200[CH02] 0x010[B group] 0x00E[pin 14]
	ADC2_CH03_B13		= 0x31D,													// 0x300[CH03] 0x010[B group] 0x00D[pin 13]
	ADC2_CH04_A04		= 0x404,													// 0x400[CH04] 0x000[A group] 0x004[pin  4]
	ADC2_CH05_A05		= 0x505,													// 0x500[CH05] 0x000[A group] 0x005[pin  5]
	ADC2_CH06_A06		= 0x606,													// 0x600[CH06] 0x000[A group] 0x006[pin  6]
	ADC2_CH07_A07		= 0x707,													// 0x700[CH07] 0x000[A group] 0x007[pin  7]
	ADC2_CH08_B00		= 0x810,													// 0x800[CH08] 0x010[B group] 0x000[pin  0]
	ADC2_CH09_B01		= 0x911,													// 0x900[CH09] 0x010[B group] 0x001[pin  1]
	ADC2_CH10_B02		= 0xA12,													// 0xA00[CH10] 0x010[B group] 0x002[pin  2]
	ADC2_CH11_B10		= 0xB1A,													// 0xB00[CH11] 0x010[B group] 0x00A[pin 10]

	ADC_CHE_TEMP		= 0xE20,													// 温度传感器通道
	ADC_CHF_VS			= 0xF20,													// 内部参考电压通道
}ADCCH_enum;

// 枚举 ADC 分辨率	此枚举定义不允许用户修改
typedef enum																		// 枚举ADC通道
{
	ADC_12BIT,																		//12位分辨率
	ADC_11BIT,																		//11位分辨率
	ADC_10BIT,																		//10位分辨率
	ADC_9BIT,																		//9位分辨率
	ADC_8BIT,																		//8位分辨率
}ADCRES_enum;

// 枚举 ADC 模块号	此枚举定义不允许用户修改
typedef enum
{
	ADC_1,																			// ADC1
	ADC_2,																			// ADC2
}ADCN_enum;

extern ADC_TypeDef *adc_index[2];

void	adc_init		(ADCN_enum adc, ADCCH_enum ch, ADCRES_enum resolution);
uint16	adc_convert		(ADCN_enum adc, ADCCH_enum ch);
uint16	adc_mean_filter	(ADCN_enum adc, ADCCH_enum ch, uint8 count);

#endif
