/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_ABSOLUTE_ENCODER
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
* 					SCK					查看 ABS_ENCODER_SPI_SCK_PIN 宏定义
* 					MISO				查看 ABS_ENCODER_SPI_MISO_PIN 宏定义
* 					MOSI				查看 ABS_ENCODER_SPI_MOSI_PIN 宏定义
* 					CS					查看 ABS_ENCODER_SPI_PCS_PIN 宏定义
* 					------------------------------------
********************************************************************************************************************/

#ifndef _SEEKFREE_ABSOLUTE_ENCODER_H
#define _SEEKFREE_ABSOLUTE_ENCODER_H

#include "headfile.h"

//------------------------硬件SPI--------------------------//
#define ABS_ENCODER_SPI_NUM			SPI_2
#define ABS_ENCODER_SPI_SCK_PIN		SPI2_SCK_B10
#define ABS_ENCODER_SPI_MOSI_PIN	SPI2_MOSI_B12
#define ABS_ENCODER_SPI_MISO_PIN	SPI2_MISO_B13
#define ABS_ENCODER_SPI_PCS_PIN		C6

#define ABS_ENCODER_SPI_CSN(x)		((x == 0)? (GPIO_PIN_RESET(ABS_ENCODER_SPI_PCS_PIN))  :(GPIO_PIN_SET(ABS_ENCODER_SPI_PCS_PIN)))
//------------------------硬件SPI--------------------------//

//角度传感器参数
#define ABS_ENCODER_SPI_W		0x80
#define ABS_ENCODER_SPI_R		0x40

#define ZERO_L_REG				0x00
#define ZERO_H_REG				0x01
#define DIR_REG					0X09

uint16	encoder_angle_spi		(void);
int16	encoder_speed_spi		(void);
void	encoder_init_spi		(void);

#endif
