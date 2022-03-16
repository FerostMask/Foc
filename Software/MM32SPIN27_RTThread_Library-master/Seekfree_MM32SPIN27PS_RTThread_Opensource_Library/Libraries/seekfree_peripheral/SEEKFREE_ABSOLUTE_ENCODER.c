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
* 					SCK					查看SEEKFREE_ABSOLUTE_ENCODER.H文件ABS_ENCODER_SPI_SCK_PIN 宏定义
* 					MISO				查看SEEKFREE_ABSOLUTE_ENCODER.H文件ABS_ENCODER_SPI_MISO_PIN宏定义
* 					MOSI				查看SEEKFREE_ABSOLUTE_ENCODER.H文件ABS_ENCODER_SPI_MOSI_PIN宏定义
* 					CS					查看SEEKFREE_ABSOLUTE_ENCODER.H文件ABS_ENCODER_SPI_PCS_PIN  宏定义
* 					------------------------------------
********************************************************************************************************************/

#include "SEEKFREE_ABSOLUTE_ENCODER.h"
#include "zf_systick.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief		通过SPI写一个byte,同时读取一个byte
// @param		byte			发送的数据
// @return		uint8			return 返回status状态
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static uint8 spi_wr_byte(uint8 byte)
{
	spi_mosi(ABS_ENCODER_SPI_NUM, &byte, &byte, 1);
	return(byte);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		将val写入cmd对应的寄存器地址,同时返回status字节
// @param		cmd				命令字
// @param		val				待写入寄存器的数值
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
//static void spi_w_reg_byte(uint8 cmd, uint8 val)
//{
//	ABS_ENCODER_SPI_CSN (0);
//	cmd |= ABS_ENCODER_SPI_W;
//	spi_wr_byte(cmd);
//	spi_wr_byte(val);
//	ABS_ENCODER_SPI_CSN (1);
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief		读取cmd所对应的寄存器地址
// @param		cmd				命令字
// @param		*val			存储读取的数据地址
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
//static void spi_r_reg_byte(uint8 cmd, uint8 *val)
//{
//	ABS_ENCODER_SPI_CSN (0);
//	cmd |= ABS_ENCODER_SPI_R;
//	spi_wr_byte(cmd);
//	*val = spi_wr_byte(0);
//	ABS_ENCODER_SPI_CSN (1);
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief		写入一个数据到编码器的寄存器
// @param		cmd				寄存器地址
// @param		*val			写入数据的地址
// @return		uint8			0：程序  1：失败
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static uint8 encoder_spi_w_reg_byte(uint8 cmd, uint8 val)
{
	uint8 dat;
	ABS_ENCODER_SPI_CSN (0);
	cmd |= ABS_ENCODER_SPI_W;
	spi_wr_byte(cmd);
	spi_wr_byte(val);
	ABS_ENCODER_SPI_CSN (1);
	systick_delay_us(1);
	ABS_ENCODER_SPI_CSN (0);
	dat = spi_wr_byte(0x00);
	spi_wr_byte(0x00);
	ABS_ENCODER_SPI_CSN (1);

	if(val != dat)  return 1;														// 写入失败
	return 0;																		// 写入成功
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		读取寄存器
// @param		cmd				寄存器地址
// @param		*val			存储读取的数据地址
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void encoder_spi_r_reg_byte(uint8 cmd, uint8 *val)
{
	ABS_ENCODER_SPI_CSN (0);
	cmd |= ABS_ENCODER_SPI_R;
	spi_wr_byte(cmd);
	spi_wr_byte(0x00);

	ABS_ENCODER_SPI_CSN (1);
	systick_delay_us(1);
	ABS_ENCODER_SPI_CSN (0);
	*val = spi_wr_byte(0x00);
	spi_wr_byte(0x00);
	ABS_ENCODER_SPI_CSN (1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		设置零偏
// @param		zero_position	需要设置的零偏
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void set_zero_position_spi(uint16 zero_position)
{
	zero_position = (uint16)(4096 - zero_position);
	zero_position = zero_position << 4;
	encoder_spi_w_reg_byte(ZERO_L_REG,(uint8)zero_position);						// 设置零位
	encoder_spi_w_reg_byte(ZERO_H_REG,zero_position>>8);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		编码器自检函数
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void encoder_self5_check(void)
{
	uint8 val;
	do
	{
		encoder_spi_r_reg_byte(6,&val);
		//卡在这里原因有以下几点
		//1 编码器坏了，如果是新的这样的概率极低
		//2 接线错误或者没有接好
	}while(0x1C != val);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		读取当前角度
// @param		void
// @return		uint16			返回角度值0-4096 对应0-360°
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint16 encoder_angle_spi(void)
{
	uint16 angle;
	angle = 0;
	ABS_ENCODER_SPI_CSN (0);
	angle = (uint16)spi_wr_byte(0x00);      
	angle <<= 8;																	// 存储高八位
	angle |= (uint16)spi_wr_byte(0x00);												// 存储低八位
	ABS_ENCODER_SPI_CSN (1);   

	return angle>>4;																// 12位精度，因此右移四位
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		编码器获取
// @param		void
// @return		uint16			返回角度值0-1023
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
int16 encoder_speed_spi(void)
{
	static uint16 old_angle ,new_angle;
	uint16 angle_diff;

	//获取本次角度数据
	ABS_ENCODER_SPI_CSN (0);
	new_angle = (uint16)spi_wr_byte(0x00);
	new_angle <<= 8;																// 存储高八位
	new_angle |= (uint16)spi_wr_byte(0x00);											// 存储低八位
	ABS_ENCODER_SPI_CSN (1);
	new_angle >>= 6;

	angle_diff = (old_angle - new_angle)&0x3ff;
	old_angle = new_angle;															// 保存这次

	return ((int16)angle_diff - ((angle_diff & 0x200)<<1));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		编码器初始化函数
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void encoder_init_spi(void)
{
	//CS引脚初始化
	gpio_init(ABS_ENCODER_SPI_PCS_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	//初始化SPI，使用软件控制CS引脚
	spi_init(ABS_ENCODER_SPI_NUM, ABS_ENCODER_SPI_SCK_PIN, ABS_ENCODER_SPI_MOSI_PIN, ABS_ENCODER_SPI_MISO_PIN, SPI_NSS_NULL, 0, SystemCoreClock/32);
	encoder_self5_check();
	encoder_spi_w_reg_byte(DIR_REG,0x00);											// 设置旋转方向 正转数值变小：0x00   反转数值变大：0x80
	set_zero_position_spi(0);														// 设置零偏
}

