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

#include "zf_gpio.h"
#include "zf_systick.h"
#include "SEEKFREE_IIC.h"

//内部数据定义
uint8 IIC_ad_main;															// 器件从地址
uint8 IIC_ad_sub;															// 器件子地址
uint8 *IIC_buf;																// 发送|接收数据缓冲区
uint8 IIC_num;																// 发送|接收数据个数

static uint16 simiic_delay_time=5;										// ICM等传感器应设置为100

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC延时 时间设置
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
//static void simiic_delay_set(uint16 time)
//{
//	simiic_delay_time = time;												// ICM等传感器应设置为100
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC延时 时间设置
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void simiic_delay(void)
{
	uint16 delay_time;
	delay_time = simiic_delay_time;
	while(delay_time--);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC起始信号
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void simiic_start(void)
{
	SDA1();
	SCL1();
	simiic_delay();
	SDA0();
	simiic_delay();
	SCL0();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC停止信号
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void simiic_stop(void)
{
	SDA0();
	SCL0();
	simiic_delay();
	SCL1();
	simiic_delay();
	SDA1();
	simiic_delay();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC ACK信号
// @param		ack_dat			主应答 (包含ack:SDA=0和no_ack:SDA=0)
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void simiic_sendack(unsigned char ack_dat)
{
	SCL0();
	simiic_delay();
	if(ack_dat)
		SDA0();
	else
		SDA1();

	SCL1();
	simiic_delay();
	SCL0();
	simiic_delay();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC等待应答
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static int sccb_waitack(void)
{
	SCL0();
	DIR_IN();
	simiic_delay();

	SCL1();
	simiic_delay();

	if(SDA)																	// 应答为高电平，异常，通信失败
	{
		DIR_OUT();
		SCL0();
		return 0;
	}
	DIR_OUT();
	SCL0();
	simiic_delay();
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 字节发送 送完后接收从应答 不考虑从应答位
// @param		c				发送c(可以是数据也可是地址)
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void send_ch(uint8 c)
{
	uint8 i = 8;
	while(i--)
	{
	if(c & 0x80)
		SDA1();																		// SDA 输出数据
	else
		SDA0();
	c <<= 1;
	simiic_delay();
	SCL1();																			// SCL 拉高，采集信号
	simiic_delay();
	SCL0();																			// SCL 时钟线拉低
	}
	sccb_waitack();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 字节接收 此程序应配合|主应答函数|使用
// @param		ack_x			应答
// @return		uint8			数据
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static uint8 read_ch(uint8 ack_x)
{
	uint8 i;
	uint8 c;
	c=0;
	SCL0();
	simiic_delay();
	SDA1();             
	DIR_IN();																		// 置数据线为输入方式
	for(i=0;i<8;i++)
	{
		simiic_delay();
		SCL0();																		// 置时钟线为低，准备接收数据位
		simiic_delay();
		SCL1();																		// 置时钟线为高，使数据线上数据有效
		simiic_delay();
		c<<=1;
		if(SDA)
			c+=1;																	// 读数据位，将接收的数据存c
	}
	DIR_OUT();
	SCL0();
	simiic_delay();
	simiic_sendack(ack_x);

	return c;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC写数据到设备寄存器函数
// @param		dev_add			设备地址(低七位地址)
// @param		reg				寄存器地址
// @param		dat				写入的数据
// @return		void						
// @since		v1.0
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void simiic_write_reg(uint8 dev_add, uint8 reg, uint8 dat)
{
	simiic_start();
	send_ch( (dev_add<<1) | 0x00);													// 发送器件地址加写位
	send_ch( reg );																	// 发送从机寄存器地址
	send_ch( dat );																	// 发送需要写入的数据
	simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC从设备寄存器读取数据
// @param		dev_add			设备地址(低七位地址)
// @param		reg				寄存器地址
// @param		type			选择通信方式是IIC  还是 SCCB
// @return		uint8			返回寄存器的数据
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 simiic_read_reg(uint8 dev_add, uint8 reg, IIC_type type)
{
	uint8 dat;
	simiic_start();
	send_ch( (dev_add<<1) | 0x00);													// 发送器件地址加写位
	send_ch( reg );																	// 发送从机寄存器地址
	if(type == SCCB)simiic_stop();

	simiic_start();
	send_ch( (dev_add<<1) | 0x01);													// 发送器件地址加读位
	dat = read_ch(SEEKFREE_NACK);													// 读取数据
	simiic_stop();

	return dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC读取多字节数据
// @param		dev_add			设备地址(低七位地址)
// @param		reg				寄存器地址
// @param		dat_add			数据保存的地址指针
// @param		num				读取字节数量
// @param		type			选择通信方式是IIC  还是 SCCB
// @return		uint8			返回寄存器的数据
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void simiic_read_regs(uint8 dev_add, uint8 reg, uint8 *dat_add, uint8 num, IIC_type type)
{
	simiic_start();
	send_ch( (dev_add<<1) | 0x00);													//发送器件地址加写位
	send_ch( reg );																	//发送从机寄存器地址
	if(type == SCCB)simiic_stop();

	simiic_start();
	send_ch( (dev_add<<1) | 0x01);													//发送器件地址加读位
	while(--num)
	{
		*dat_add = read_ch(SEEKFREE_ACK);											//读取数据
		dat_add++;
	}
	*dat_add = read_ch(SEEKFREE_NACK);												//读取数据
	simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC端口初始化
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void simiic_init(void)
{
	gpio_init(SEEKFREE_SCL, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	gpio_init(SEEKFREE_SDA, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}
