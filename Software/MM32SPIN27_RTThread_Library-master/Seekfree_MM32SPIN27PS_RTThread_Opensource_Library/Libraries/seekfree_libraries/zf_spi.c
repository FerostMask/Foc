/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file				spi
 * @company			成都逐飞科技有限公司
 * @author			逐飞科技(QQ3184284598)
 * @version			查看doc内version文件 版本说明
 * @Software			IAR 8.3 or MDK 5.24
 * @Target core		MM32SPIN2XPs
 * @Taobao			https://seekfree.taobao.com/
 * @date				2020-11-23
 ********************************************************************************************************************/

#include "zf_spi.h"
#include "zf_gpio.h"

SPI_TypeDef *spi_index[2] = {SPI1, SPI2};

//-------------------------------------------------------------------------------------------------------------------
// @brief		SPI 引脚初始化 内部调用
// @param		tx_pin			选择 TX 引脚
// @param		rx_pin			选择 RX 引脚
// @param		rx_pin			选择 RX 引脚
// @return		void			NULL
// Sample usage:				spi_pin_init(sck_pin, mosi_pin, miso_pin, cs_pin);
//-------------------------------------------------------------------------------------------------------------------
static void spi_pin_init(SPI_SCKPIN_enum sck_pin, SPI_MOSIPIN_enum mosi_pin, SPI_MISOPIN_enum miso_pin, SPI_NSSPIN_enum cs_pin)
{
	afio_init((PIN_enum)(sck_pin & 0xff), GPO, (GPIOAF_enum)((sck_pin & 0xf00) >> 8), GPO_AF_PUSH_PUL);	  // 提取对应IO索引 AF功能编码
	afio_init((PIN_enum)(mosi_pin & 0xff), GPO, (GPIOAF_enum)((mosi_pin & 0xf00) >> 8), GPO_AF_PUSH_PUL); // 提取对应IO索引 AF功能编码
	if (SPI_MISO_NULL != miso_pin)
		afio_init((PIN_enum)(miso_pin & 0xff), GPI, (GPIOAF_enum)((miso_pin & 0xf00) >> 8), GPI_PULL_UP); // 提取对应IO索引 AF功能编码
	if (SPI_NSS_NULL != cs_pin)
		afio_init((PIN_enum)(cs_pin & 0xff), GPO, (GPIOAF_enum)((cs_pin & 0xf00) >> 8), GPO_AF_PUSH_PUL); // 提取对应IO索引 AF功能编码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		SPI初始化
// @param		spi_n			选择SPI模块 (SPI_1-SPI_2)
// @param		cs_pin			选择SPI片选引脚
// @param		sck_pin			选择SPI时钟引脚
// @param		mosi_pin		选择SPI MOSI引脚
// @param		miso_pin		选择SPI MISO引脚
// @param		mode			SPI模式	0：CPOL=0 CPHA=0	1：CPOL=0 CPHA=1	2：CPOL=1 CPHA=0	3：CPOL=1 CPHA=1		//具体细节可自行百度
// @param		baud			设置SPI的波特率
// @return		void
// Sample usage:				spi_init(SPI_1, SPI1_SCK_A04, SPI1_MOSI_A07, SPI1_MISO_A06, SPI1_NSS_A04, 0, 1*1000*1000);	//硬件SPI初始化  模式0 波特率为1Mhz
//-------------------------------------------------------------------------------------------------------------------
void spi_init(SPIN_enum spi_n, SPI_SCKPIN_enum sck_pin, SPI_MOSIPIN_enum mosi_pin, SPI_MISOPIN_enum miso_pin, SPI_NSSPIN_enum cs_pin, uint8 mode, uint32 baud)
{
	spi_pin_init(sck_pin, mosi_pin, miso_pin, cs_pin); // 初始化相关引脚

	if (spi_n == SPI_1)
		RCC->APB2ENR |= RCC_APB2ENR_SPI1;
	else
		RCC->APB1ENR |= RCC_APB1ENR_SPI2;

	spi_index[spi_n]->GCTL |= SPI_GCR_MODE;	  // 主机模式
	spi_index[spi_n]->CCTL |= SPI_CCR_SPILEN; // 8bits 数据
	switch (mode)
	{
	case 0:
		spi_index[spi_n]->CCTL &= ~SPI_CCR_CPOL; // SCK 空闲时低电平
		spi_index[spi_n]->CCTL |= SPI_CCR_CPHA;	 // 第一个时钟边沿开始采样
		break;
	case 1:
		spi_index[spi_n]->CCTL &= ~SPI_CCR_CPOL; // SCK 空闲时低电平
		spi_index[spi_n]->CCTL &= ~SPI_CCR_CPHA; // 第二个时钟边沿开始采样
		break;
	case 2:
		spi_index[spi_n]->CCTL |= SPI_CCR_CPOL; // SCK 空闲时高电平
		spi_index[spi_n]->CCTL |= SPI_CCR_CPHA; // 第一个时钟边沿开始采样
		break;
	case 3:
		spi_index[spi_n]->CCTL |= SPI_CCR_CPOL;	 // SCK 空闲时高电平
		spi_index[spi_n]->CCTL &= ~SPI_CCR_CPHA; // 第二个时钟边沿开始采样
		break;
	}
	spi_index[spi_n]->GCTL &= ~SPI_GCR_NSS;				   // 片选软件控制
	spi_index[spi_n]->CCTL |= SPI_CCR_RXEDGE;			   // 在传输数据位的尾时钟沿采样数据 用于高速模式
	spi_index[spi_n]->SPBRG = (SystemCoreClock / baud);	   // 设置波特率
	spi_index[spi_n]->CCTL &= ~SPI_CCR_LSBFE;			   // MSB
	spi_index[spi_n]->GCTL |= SPI_GCR_TXEN | SPI_GCR_RXEN; // 使能 TX/RX
	spi_index[spi_n]->GCTL |= SPI_GCR_SPIEN;			   // 使能
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		SPI发送接收函数
// @param		spi_n			选择SPI模块 (SPI_1-SPI_2)
// @param		modata			发送的数据缓冲区地址
// @param		midata			发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// @param		len				发送的字节数
// @param		continuous		本次通信是CS是否持续保持有效状态 1:持续保持 0:每传输完一个字节关闭CS(一般设置为1 即可)
// @return		void
// @since		v2.0
// Sample usage:				spi_mosi(SPI_1,buf,buf,1);										//发送buff的内容，并接收到buf里，长度为1字节 通信期间CS持续拉低
//-------------------------------------------------------------------------------------------------------------------
void spi_mosi(SPIN_enum spi_n, uint8 *modata, uint8 *midata, uint32 len)
{
	while (!(spi_index[spi_n]->CSTAT & SPI_SR_TXEPT))
		;		  // 等待发送为空
	while (len--) // 判断长度
	{
		spi_index[spi_n]->TXREG = *modata++; // 发送数据
		while (!(spi_index[spi_n]->CSTAT & SPI_SR_TXEPT))
			;				// 等待发送为空
		if (midata != NULL) // 接收不为空
		{
			while (!(spi_index[spi_n]->CSTAT & SPI_SR_RXAVL))
				;								 // 发送没满
			*midata++ = spi_index[spi_n]->RXREG; // 读取数据
		}
	}
}
