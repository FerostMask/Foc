/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_TSL1401
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					接线定义：
* 					------------------------------------
* 					模块管脚			单片机管脚
* 					CCD_AO				查看SEEKFREE_TSL1401.h文件中的AD_CHANNEL 宏定义
* 					CCD_CLK				查看SEEKFREE_TSL1401.h文件中的CCD_CLK_PIN宏定义
* 					CCD_SI				查看SEEKFREE_TSL1401.h文件中的CCD_SI_PIN 宏定义
* 					------------------------------------
********************************************************************************************************************/

#ifndef _TSL1401_c
#define _TSL1401_c

#include "common.h"
#include "zf_adc.h"
#include "zf_gpio.h"
#include "zf_uart.h"
#include "zf_pit.h"

#define CCD_NUM				2
#define CCD_CLK_PIN			C2												//	CCD的CLK引脚定义
#define CCD_SI_PIN			C3												//	CCD的SI 引脚定义
#define AD_GROUP			ADC_2											//	定义AD采集通道
#define AD_CHANNEL			{ADC2_CH02_B14,ADC2_CH01_B15}					//	定义AD采集通道
#define EXPOSURE_TIME		10												//	定义CCD曝光时间 单位MS
#define PIT_TIMER_CH		TIM_14											//	使用周期中断
#define AD_RESOLUTION		ADC_10BIT										//	AD转换10位
	
#define CCD_CLK(x)			gpio_set(CCD_CLK_PIN, x)
#define CCD_SI(x)			gpio_set(CCD_SI_PIN, x)

extern uint16 ccd_data[CCD_NUM][128];										// CCD数据
extern uint8 tsl1401_finish_flag;

void ccd_init(void);
void ccd_collect(void);
void ccd_send_data(UARTN_enum uart_n ,uint16 *dat);

#endif
