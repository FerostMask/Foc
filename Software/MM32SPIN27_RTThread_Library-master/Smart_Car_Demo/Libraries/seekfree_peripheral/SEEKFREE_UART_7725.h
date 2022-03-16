/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_UART_7725
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
* 					RXD(51的RX)			查看SEEKFREE_UART_7725.h文件中的OV7725_UART_COF_UART_TX宏定义
* 					TXD(51的TX)			查看SEEKFREE_UART_7725.h文件中的OV7725_UART_COF_UART_RX宏定义
* 					场中断(VSY)			查看SEEKFREE_UART_7725.h文件中的OV7725_UART_VSYNC_PIN宏定义
* 					行中断(HREF)		查看SEEKFREE_UART_7725.h文件中的OV7725_UART_HREF_PIN宏定义
* 					像素中断(PCLK)		查看SEEKFREE_UART_7725.h文件中的OV7725_UART_PCLK_PIN宏定义
* 					数据口(D0-D7)		查看SEEKFREE_UART_7725.h文件中的OV7725_UART_DATA_PIN宏定义
* 					------------------------------------
********************************************************************************************************************/

#ifndef _SEEKFREE_UART_7725_h
#define _SEEKFREE_UART_7725_h

#include "common.h"
#include "zf_uart.h"

#define OV7725_UART_W					160
#define OV7725_UART_H					120

#define OV7725_UART_SIZE				(OV7725_UART_W * OV7725_UART_H /8)
#define OV7725_UART_DMA_NUM				(OV7725_UART_SIZE )

#define OV7725_UART_COF_UART			UART_2								// 小钻风配置串口
#define OV7725_UART_COF_UART_TX			UART2_TX_C04						// 小钻风TX串口引脚
#define OV7725_UART_COF_UART_RX			UART2_RX_C05						// 小钻风RX串口引脚

#define OV7725_UART_DMA_CH				DMA1_Channel4						// TIM触发DMA通道禁止随意修改
#define OV7725_UART_DMA_IRQN			DMA1_Channel4_5_IRQn				// DMA中断通道

#define OV7725_UART_PCLK				TIM_3_ETR_D06						// GPIO触发TIM引脚禁止随意修改

#define OV7725_UART_VSYNC_PIN			D7									// 场中断引脚
#define OV7725_UART_VSYNC_PINT			EXTI_Line7							// 场中断所使用的PINT中断通道
#define OV7725_UART_VSYNC_IRQN			EXTI4_15_IRQn						// 中断号

#define OV7725_UART_DATA_PIN			B0
#define OV7725_UART_DATA_ADD			GPIO_IDR_ADDR(OV7725_UART_DATA_PIN)

typedef enum
{
	OV7725_INIT			= 0x00,
	OV7725_RESERVE,
	OV7725_CONTRAST,
	OV7725_FPS,
	OV7725_COL,
	OV7725_ROW,
	OV7725_CONFIG_FINISH,

	OV7725_GET_WHO_AM_I	= 0xEF,
	OV7725_GET_STATUS	= 0xF1,
	OV7725_GET_VERSION	= 0xF2,

	OV7725_SET_ADDR		= 0xFE,
	OV7725_SET_DATA		= 0xFF,
}OV7725_CMD;

extern uint8_t ov7725_uart_finish_flag;
extern uint8_t ov7725_uart_image_bin [OV7725_UART_H][OV7725_UART_W/8];
//extern uint8_t ov7725_uart_image_bin [OV7725_UART_SIZE];

void	ov7725_uart_dma				(void);
void	ov7725_uart_vsync			(void);
void	ov7725_uart_exti_init		(void);
void	ov7725_uart_tim_etr_init	(void);
void	ov7725_uart_dma_init		(void);
uint8_t	ov7725_uart_init			(void);
void	ov7725_cof_uart_interrupt	(void);
void	image_decompression			(uint8 *data1,uint8 *data2);
void	seekfree_sendimg_7725		(UARTN_enum uartn, void *imgaddr, uint32 imgsize);

#endif
