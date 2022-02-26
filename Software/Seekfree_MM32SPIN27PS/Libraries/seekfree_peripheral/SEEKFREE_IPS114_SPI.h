/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_IPS114_SPI
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					接线定义：
* 					------------------------------------
* 					模块管脚				单片机管脚
* 					SCL					查看IPS114_SCL_PIN宏定义的引脚	硬件SPI引脚不可随意切换
* 					SDA					查看IPS114_SDA_PIN宏定义的引脚	硬件SPI引脚不可随意切换
* 					RES					查看IPS114_REST_PIN宏定义的引脚
* 					DC					查看IPS114_DC_PIN宏定义的引脚
* 					CS					查看IPS114_CS_PIN宏定义的引脚		硬件SPI引脚不可随意切换
* 					电源引脚
* 					BL  3.3V电源（背光控制引脚，也可以接PWM来控制亮度）
* 					VCC 3.3V电源
* 					GND 电源地
* 					最大分辨率135*240
* 					------------------------------------
********************************************************************************************************************/


#ifndef _SEEKFREE_IPS114_SPI_H
#define _SEEKFREE_IPS114_SPI_H


#include "common.h"
#include "SEEKFREE_FONT.h"


//-----------------引脚定义------------------------------
#define IPS114_SPIN				SPI_2										// 定义使用的SPI号
#define IPS114_SCL_PIN			SPI2_SCK_C11								// 定义SPI_SCK引脚
#define IPS114_SDA_PIN			SPI2_MOSI_C12								// 定义SPI_MOSI引脚

#define IPS114_REST_PIN			C13											// 液晶复位引脚定义 由于实际通讯未使用B2因此 这里复用为GPIO
#define IPS114_DC_PIN			C14											// 液晶命令位引脚定义
#define IPS114_CS_PIN			C15											// 定义SPI_CS引脚
#define IPS114_BL_PIN			C10											// 液晶背光引脚定义

#define IPS114_DC(x)			((x == 0)? (GPIO_PIN_RESET(IPS114_DC_PIN))  :(GPIO_PIN_SET(IPS114_DC_PIN)))
#define IPS114_REST(x)			((x == 0)? (GPIO_PIN_RESET(IPS114_REST_PIN)):(GPIO_PIN_SET(IPS114_REST_PIN)))
#define IPS114_CS(x)			((x == 0)? (GPIO_PIN_RESET(IPS114_CS_PIN))  :(GPIO_PIN_SET(IPS114_CS_PIN)))
#define IPS114_BL(x)			((x == 0)? (GPIO_PIN_RESET(IPS114_BL_PIN))  :(GPIO_PIN_SET(IPS114_BL_PIN)))

//定义写字笔的颜色
#define IPS114_PENCOLOR			RED

//定义背景颜色
#define IPS114_BGCOLOR			WHITE

#define IPS114_W				135
#define IPS114_H				240

//定义显示方向
//0 竖屏模式
//1 竖屏模式  旋转180
//2 横屏模式
//3 横屏模式  旋转180
#define IPS114_DISPLAY_DIR 3

#if (0==IPS114_DISPLAY_DIR || 1==IPS114_DISPLAY_DIR)
#define	IPS114_X_MAX			IPS114_W									//液晶X方宽度
#define IPS114_Y_MAX			IPS114_H									//液晶Y方宽度

#elif (2==IPS114_DISPLAY_DIR || 3==IPS114_DISPLAY_DIR)
#define	IPS114_X_MAX			IPS114_H									//液晶X方宽度
#define IPS114_Y_MAX			IPS114_W 									//液晶Y方宽度

#else
#error "IPS114_DISPLAY_DIR 定义错误"
#endif

void	ips114_init						(void);
void	ips114_clear					(uint16 color);
void	ips114_drawpoint				(uint16 x,uint16 y,uint16 color);
void	ips114_showchar					(uint16 x,uint16 y,const int8 dat);
void	ips114_showstr					(uint16 x,uint16 y,const int8 dat[]);
void	ips114_showint8					(uint16 x,uint16 y,int8 dat);
void	ips114_showuint8				(uint16 x,uint16 y,uint8 dat);
void	ips114_showint16				(uint16 x,uint16 y,int16 dat);
void	ips114_showuint16				(uint16 x,uint16 y,uint16 dat);
void	ips114_showint32				(uint16 x,uint16 y,int32 dat,uint8 num);
void	ips114_showfloat				(uint16 x,uint16 y,double dat,uint8 num,uint8 pointnum);
void	ips114_displayimage032			(uint8 *p, uint16 width, uint16 height);
void	ips114_displayimage032_zoom		(uint8 *p, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void	ips114_displayimage032_zoom1	(uint8 *p, uint16 width, uint16 height, uint16 start_x, uint16 start_y, uint16 dis_width, uint16 dis_height);
void	ips114_displayimage8660_zoom	(uint16 *p, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void	ips114_displayimage8660_zoom1	(uint8 *p, uint16 width, uint16 height, uint16 start_x, uint16 start_y, uint16 dis_width, uint16 dis_height);
void	ips114_displayimage7725			(uint8 *p, uint16 width, uint16 height);
void	ips114_display_chinese			(uint16 x, uint16 y, uint8 size, const uint8 *p, uint8 number, uint16 color);

#endif
