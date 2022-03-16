/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_7725
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
* 					SDA					A9
* 					SCL					A8
* 					场中断(VSY)			A0
* 					行中断(HREF)		未使用，因此不接
* 					像素中断(PCLK)		A1
* 	                数据口(D0-D7)		B24-B31
* 					------------------------------------
********************************************************************************************************************/

#include "zf_exti.h"
#include "zf_systick.h"
#include "zf_uart.h"
#include "zf_gpio.h"
#include "zf_camera.h"

#include "SEEKFREE_7725.h"

uint8_t ov7725_finish_flag = 0;
uint8_t ov7725_image_bin [OV7725_W][OV7725_H/8];
static uint16 ov7725_simiic_delay_time=100;											// ICM等传感器应设置为100

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC延时 时间设置
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
//static void ov7725_simiic_delay_set(uint16 time)
//{
//	ov7725_simiic_delay_time = time;												// ICM等传感器应设置为100
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC延时 时间设置
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_delay(void)
{
	uint16 delay_time;
	delay_time = ov7725_simiic_delay_time;
	while(delay_time--);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC起始信号
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_start(void)
{
	OV7725_SDA1();
	OV7725_SCL1();
	ov7725_simiic_delay();
	OV7725_SDA0();
	ov7725_simiic_delay();
	OV7725_SCL0();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC停止信号
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_stop(void)
{
	OV7725_SDA0();
	OV7725_SCL0();
	ov7725_simiic_delay();
	OV7725_SCL1();
	ov7725_simiic_delay();
	OV7725_SDA1();
	ov7725_simiic_delay();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC ACK信号
// @param		ack_dat			主应答 (包含ack:SDA=0和no_ack:SDA=0)
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_sendack(unsigned char ack_dat)
{
	OV7725_SCL0();
	ov7725_simiic_delay();
	if(ack_dat)
		OV7725_SDA0();
	else
		OV7725_SDA1();

	OV7725_SCL1();
	ov7725_simiic_delay();
	OV7725_SCL0();
	ov7725_simiic_delay();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC等待应答
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static int ov7725_sccb_waitack(void)
{
	OV7725_SCL0();
	OV7725_DIR_IN();
	ov7725_simiic_delay();

	OV7725_SCL1();
	ov7725_simiic_delay();

	if(OV7725_SDA)																	// 应答为高电平，异常，通信失败
	{
		OV7725_DIR_OUT();
		OV7725_SCL0();
		return 0;
	}
	OV7725_DIR_OUT();
	OV7725_SCL0();
	ov7725_simiic_delay();
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 字节发送 送完后接收从应答 不考虑从应答位
// @param		c				发送c(可以是数据也可是地址)
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_send_ch(uint8 c)
{
	uint8 i = 8;
	while(i--)
	{
		if(c & 0x80)
			OV7725_SDA1();															// SDA 输出数据
		else
			OV7725_SDA0();
		c <<= 1;
		ov7725_simiic_delay();
		OV7725_SCL1();																// SCL 拉高，采集信号
		ov7725_simiic_delay();
		OV7725_SCL0();																// SCL 时钟线拉低
	}
	ov7725_sccb_waitack();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 字节接收 此程序应配合|主应答函数|使用
// @param		ack_x			应答
// @return		uint8			数据
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_read_ch(uint8 ack_x)
{
	uint8 i;
	uint8 c;
	c=0;
	OV7725_SCL0();
	ov7725_simiic_delay();
	OV7725_SDA1();             
	OV7725_DIR_IN();																// 置数据线为输入方式
	for(i=0;i<8;i++)
	{
		ov7725_simiic_delay();
		OV7725_SCL0();																// 置时钟线为低，准备接收数据位
		ov7725_simiic_delay();
		OV7725_SCL1();																// 置时钟线为高，使数据线上数据有效
		ov7725_simiic_delay();
		c<<=1;
		if(OV7725_SDA)
			c+=1;																	// 读数据位，将接收的数据存c
	}
	OV7725_DIR_OUT();
	OV7725_SCL0();
	ov7725_simiic_delay();
	ov7725_simiic_sendack(ack_x);

	return c;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 写数据到设备寄存器
// @param		dev_add			设备地址
// @param		reg				寄存器地址
// @param		dat				写入的数据
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_write_reg(uint8 dev_add, uint8 reg, uint8 dat)
{
	ov7725_simiic_start();
	ov7725_send_ch( (dev_add<<1) | 0x00);											// 发送器件地址加写位
	ov7725_send_ch( reg );															// 发送从机寄存器地址
	ov7725_send_ch( dat );															// 发送需要写入的数据
	ov7725_simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 从设备寄存器读取数据
// @param		dev_add			设备地址
// @param		reg				寄存器地址
// @param		type			选择通信方式是IIC  还是 SCCB
// @return		uint8			返回寄存器的数据
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static uint8 ov7725_simiic_read_reg(uint8 dev_add, uint8 reg, IIC_type type)
{
	uint8 dat;
	ov7725_simiic_start();
	ov7725_send_ch( (dev_add<<1) | 0x00);											// 发送器件地址加写位
	ov7725_send_ch( reg );															// 发送从机寄存器地址
	if(type == SCCB)ov7725_simiic_stop();

	ov7725_simiic_start();
	ov7725_send_ch( (dev_add<<1) | 0x01);											// 发送器件地址加读位
	dat = ov7725_read_ch(OV7725_NACK);												// 读取数据
	ov7725_simiic_stop();

	return dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 读取多字节数据
// @param		dev_add			设备地址
// @param		reg				寄存器地址
// @param		dat_add			数据保存的地址指针
// @param		num				读取字节数量
// @param		type			选择通信方式是IIC  还是 SCCB
// @return		uint8			返回寄存器的数据
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
//static void ov7725_simiic_read_regs (uint8 dev_add, uint8 reg, uint8 *dat_add, uint8 num, IIC_type type)
//{
//	ov7725_simiic_start();
//	ov7725_send_ch( (dev_add<<1) | 0x00);											// 发送器件地址加写位
//	ov7725_send_ch( reg );															// 发送从机寄存器地址
//	if(type == SCCB)ov7725_simiic_stop();
//
//	ov7725_simiic_start();
//	ov7725_send_ch( (dev_add<<1) | 0x01);											// 发送器件地址加读位
//	while(--num)
//	{
//		*dat_add = ov7725_read_ch(OV7725_ACK);										// 读取数据
//		dat_add++;
//	}
//	*dat_add = ov7725_read_ch(OV7725_NACK);											// 读取数据
//	ov7725_simiic_stop();
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC端口初始化
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加ov7725_simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void ov7725_simiic_init(void)
{
	gpio_init(OV7725_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	gpio_init(OV7725_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		小钻风摄像头内部寄存器初始化(内部使用，用户无需调用)
// @param		NULL
// @return		uint8			返回0则出错，返回1则成功
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_reg_init(void)
{
	uint8 ov7725_idcode = 0;

	ov7725_simiic_write_reg ( OV7725_DEV_ADD, OV7725_COM7, 0x80 );					// 复位摄像头
	systick_delay_ms(50);
	ov7725_idcode = ov7725_simiic_read_reg( OV7725_DEV_ADD, OV7725_VER ,SCCB);
	if( ov7725_idcode != OV7725_ID )    return 0;									// 校验摄像头ID号

	//ID号确认无误   然后配置寄存器
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4			, 0xC1);  
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC		, 0x01);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM2			, 0x03);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM3			, 0xD0);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM7			, 0x40);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_COM8			, 0xCE);			// 0xCE:关闭自动曝光  0xCF：开启自动曝光
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HSTART		, 0x3F);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HSIZE		, 0x50);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VSTRT		, 0x03);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VSIZE		, 0x78);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HREF			, 0x00);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SCAL0		, 0x0A);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_AWB_Ctrl0	, 0xE0);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSPAuto		, 0xff);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl2	, 0x0C);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl3	, 0x00);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl4	, 0x00);

	if(OV7725_W == 80)			ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x14);
	else if(OV7725_W == 160)	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x28);
	else if(OV7725_W == 240)	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x3c);
	else if(OV7725_W == 320)	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize, 0x50);

	if(OV7725_H == 60)			ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x1E);
	else if(OV7725_H == 120)	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x3c);
	else if(OV7725_H == 180)	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x5a);
	else if(OV7725_H == 240)	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize, 0x78);

	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28		, 0x01);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH		, 0x10);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL		, 0x1F);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM1			, 0x0c);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM2			, 0x16);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM3			, 0x2a);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM4			, 0x4e);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM5			, 0x61);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM6			, 0x6f);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM7			, 0x7b);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM8			, 0x86);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM9			, 0x8e);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM10		, 0x97);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM11		, 0xa4);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM12		, 0xaf);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM13		, 0xc5);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM14		, 0xd7);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM15		, 0xe8);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SLOP			, 0x20);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_RADI		, 0x00);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEF		, 0x13);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_XC		, 0x08);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEFB		, 0x14);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEFR		, 0x17);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_CTR		, 0x05);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_BDBase		, 0x99);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_BDMStep		, 0x03);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SDE			, 0x04);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_BRIGHT		, 0x00);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST			, 0x40);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_SIGN			, 0x06);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ0		, 0x11);
	ov7725_simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ1		, 0x02);
	return 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		小钻风摄像头初始化(调用之后设置好相关中断函数即可采集图像)
// @param		NULL
// @return		0
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_init(void)
{
	uint8 temp = 0;
	camera_type = CAMERA_BIN;														// 设置连接摄像头类型
	camera_buffer_addr = ov7725_image_bin[0];

	gpio_init(OV7725_UART_VSYNC_PIN, GPI, GPIO_LOW, GPI_PULL_UP);
	while(!temp)
	{
		temp = gpio_get(OV7725_UART_VSYNC_PIN);
	}

	ov7725_simiic_init();
	ov7725_reg_init();																// 摄像头寄存器配置

	//DMA初始化
	ov7725_uart_dma_init();
	//GPIO触发定时器初始化,PLCK引脚初始化
	ov7725_uart_tim_etr_init();
	//VSYNC初始化
	ov7725_uart_exti_init();
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ov7725 摄像头场中断
// @param		NULL
// @return		void			
// @since		v1.0
// Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void ov7725_vsync(void)
{
	OV7725_UART_DMA_CH->CNDTR = OV7725_UART_SIZE;									// 设置当前DMA传输的剩余数量，向下递减，该寄存器只能在DMA不工作时更改。
	OV7725_UART_DMA_CH->CCR |= DMA_CCR_EN;											// 开启DMA1
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ov7725 摄像头DMA完成中断
// @param		NULL
// @return		void			
// @since		v1.0
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void ov7725_dma(void)
{
	OV7725_UART_DMA_CH->CCR &= (uint16)(~DMA_CCR_EN);								//关闭DMA1
	ov7725_finish_flag = 1;															//摄像头数据采集置1
}
