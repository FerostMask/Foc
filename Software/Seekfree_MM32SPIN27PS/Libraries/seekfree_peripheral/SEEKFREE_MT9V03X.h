/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_MT9V03X
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
* 					SDA(51的RX)			查看SEEKFREE_MT9V03X.h文件中的MT9V03X_COF_UART_TX宏定义
* 					SCL(51的TX)			查看SEEKFREE_MT9V03X.h文件中的MT9V03X_COF_UART_RX宏定义
* 					场中断(VSY)			查看SEEKFREE_MT9V03X.h文件中的MT9V03X_VSYNC_PIN宏定义
* 					行中断(HREF)		查看SEEKFREE_MT9V03X.h文件中的MT9V03X_HREF_PIN宏定义
* 					像素中断(PCLK)		查看SEEKFREE_MT9V03X.h文件中的MT9V03X_PCLK_PIN宏定义
* 					数据口(D0-D7)		查看SEEKFREE_MT9V03X.h文件中的MT9V03X_DATA_PIN宏定义
* 					------------------------------------
********************************************************************************************************************/

#ifndef _SEEKFREE_MT9V03X_h
#define _SEEKFREE_MT9V03X_h

#include "common.h"
#include "zf_uart.h"

//配置摄像头参数
// MM32SPIN27 毕竟只是个 M0 内核的单片机 SRAM 只有 12KB 采集不了完整分辨率的图像
// 188*50=9400 大概是 9.18KB
#define MT9V03X_W               188		// 图像宽度  范围1-752
#define MT9V03X_H               50		// 图像高度	范围1-480

//--------------------------------------------------------------------------------------------------
//引脚配置
//--------------------------------------------------------------------------------------------------
#define MT9V03X_COF_UART		UART_2										// 配置摄像头所使用到的串口
#define MT9V03X_COF_UART_TX		UART2_TX_C04
#define MT9V03X_COF_UART_RX		UART2_RX_C05

#define MT9V03X_DMA_CH			DMA1_Channel4
#define MT9V03X_DMA_IRQN		DMA1_Channel4_5_IRQn

#define MT9V03X_TIMETR_PCLK		TIM_3_ETR_D06								// GPIO触发TIM引脚禁止随意修改

#define MT9V03X_VSYNC_PIN		D7											// 场中断引脚
#define MT9V03X_VSYNC_PINT		EXTI_Line7									// 场中断所使用的PINT中断通道
#define MT9V03X_VSYNC_IRQN		EXTI4_15_IRQn								// 中断号

#define MT9V03X_DATA_PIN		B0
#define MT9V03X_DATA_ADD		GPIO_IDR_ADDR(MT9V03X_DATA_PIN)

                        
//摄像头命令枚举
typedef enum
{
	INIT = 0,																// 摄像头初始化命令
	AUTO_EXP,																// 自动曝光命令
	EXP_TIME,																// 曝光时间命令
	FPS,																	// 摄像头帧率命令
	SET_COL,																// 图像列命令
	SET_ROW,																// 图像行命令
	LR_OFFSET,																// 图像左右偏移命令
	UD_OFFSET,																// 图像上下偏移命令
	GAIN,																	// 图像偏移命令
	CONFIG_FINISH,															// 非命令位，主要用来占位计数

	COLOR_GET_WHO_AM_I = 0xEF,
	SET_EXP_TIME = 0XF0,													// 单独设置曝光时间命令
	GET_STATUS,																// 获取摄像头配置命令
	GET_VERSION,															// 固件版本号命令

	SET_ADDR = 0XFE,														// 寄存器地址命令
	SET_DATA																// 寄存器数据命令
}CMD;

extern uint8    mt9v03x_finish_flag;//一场图像采集完成标志位
extern uint8    mt9v03x_image[MT9V03X_H][MT9V03X_W];

void set_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2]);
void get_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2]);
uint16 get_version(UARTN_enum uartn);
uint16 set_exposure_time(UARTN_enum uartn, uint16 light);

void mt9v03x_init(void);
void mt9v03x_vsync(void);
void mt9v03x_dma(void);
void mt9v03x_uart_callback(void);
void seekfree_sendimg_03x(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height);

#endif
