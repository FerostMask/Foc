/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_IIC
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
* 					SCL					查看SEEKFREE_IIC文件内的SEEKFREE_SCL宏定义
* 					SDA					查看SEEKFREE_IIC文件内的SEEKFREE_SDA宏定义
* 					------------------------------------
********************************************************************************************************************/

#ifndef _SEEKFREE_IIC_h
#define _SEEKFREE_IIC_h

#include "common.h"

// 软件 IIC 定义
#define SEEKFREE_SCL	D3															// 定义SCL引脚  可任意更改为其他IO
#define SEEKFREE_SDA	D4															// 定义SDA引脚  可任意更改为其他IO

//#define SDA				gpio_get (SEEKFREE_SDA)
#define SDA				((gpio_group[(SEEKFREE_SDA&0xf0)>>4]->IDR & (((uint16_t)0x0001) << (SEEKFREE_SDA&0x0f)))?1:0)
#define SDA0()			GPIO_PIN_RESET (SEEKFREE_SDA)								// IO口输出低电平
#define SDA1()			GPIO_PIN_SET (SEEKFREE_SDA)									// IO口输出高电平
#define SCL0()			GPIO_PIN_RESET (SEEKFREE_SCL)								// IO口输出低电平
#define SCL1()			GPIO_PIN_SET (SEEKFREE_SCL)									// IO口输出高电平
#define DIR_OUT()		gpio_dir(SEEKFREE_SDA, GPO, GPO_PUSH_PULL)					// 输出方向
#define DIR_IN()		gpio_dir(SEEKFREE_SDA, GPI, GPI_PULL_UP)					// 输入方向

#define SEEKFREE_ACK	1															// 主应答
#define SEEKFREE_NACK	0															// 从应答

typedef enum IIC																	// IIC 模块
{
	SIMIIC,
	SCCB
} IIC_type;

void	simiic_delay_set		(uint16 time);
void	simiic_start			(void);
void	simiic_stop				(void);
void	simiic_ack_main			(uint8 ack_main);
void	send_ch					(uint8 c);
uint8	read_ch					(uint8 ack);
void	simiic_write_reg		(uint8 dev_add, uint8 reg, uint8 dat);
uint8	simiic_read_reg			(uint8 dev_add, uint8 reg, IIC_type type);
void	simiic_read_regs		(uint8 dev_add, uint8 reg, uint8 *dat_add, uint8 num, IIC_type type);
void	simiic_init				(void);

#endif
