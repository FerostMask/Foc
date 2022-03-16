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

#ifndef _zf_spi_h
#define _zf_spi_h

#include "common.h"
#include "hal_rcc.h"
#include "hal_spi.h"

//此枚举定义不允许用户修改
typedef enum //枚举串口号
{
	SPI_1,
	SPI_2,
}SPIN_enum;

//枚举串口引脚	此枚举定义不允许用户修改
typedef enum
{
	SPI1_SCK_A04					= 0x104,										// 0x100[AF1] 0x000[A group] 0x004[pin  4]
	SPI1_SCK_A05					= 0x005,										// 0x000[AF0] 0x000[A group] 0x005[pin  5]
	SPI1_SCK_B03					= 0x013,										// 0x000[AF0] 0x010[B group] 0x003[pin  3]
	SPI1_SCK_C07					= 0x627,										// 0x600[AF6] 0x020[C group] 0x007[pin  7]
	SPI1_SCK_D01					= 0x631,										// 0x600[AF6] 0x030[D group] 0x001[pin  1]
	SPI1_SCK_D03					= 0x533,										// 0x500[AF5] 0x030[D group] 0x003[pin  3]

	SPI2_SCK_B10					= 0x51A,										// 0x500[AF5] 0x010[B group] 0x00A[pin 10]
	SPI2_SCK_B12					= 0x51C,										// 0x500[AF5] 0x010[B group] 0x00C[pin 12]
	SPI2_SCK_B13					= 0x51D,										// 0x500[AF5] 0x010[B group] 0x00D[pin 13]
	SPI2_SCK_B14					= 0x51E,										// 0x500[AF5] 0x010[B group] 0x00E[pin 14]
	SPI2_SCK_B15					= 0x51F,										// 0x500[AF5] 0x010[B group] 0x00F[pin 15]
	SPI2_SCK_C10					= 0x42A,										// 0x400[AF4] 0x020[C group] 0x00A[pin 10]
	SPI2_SCK_C11					= 0x52B,										// 0x500[AF5] 0x020[C group] 0x00B[pin 11]
	SPI2_SCK_C12					= 0x32C,										// 0x300[AF3] 0x020[C group] 0x00C[pin 12]
}SPI_SCKPIN_enum;

//枚举串口引脚	此枚举定义不允许用户修改
typedef enum
{
	SPI1_MOSI_A07					= 0x007,										// 0x000[AF0] 0x000[A group] 0x007[pin  7]
	SPI1_MOSI_B05					= 0x015,										// 0x000[AF0] 0x010[B group] 0x005[pin  5]
	SPI1_MOSI_C04					= 0x624,										// 0x600[AF6] 0x020[C group] 0x004[pin  4]
	SPI1_MOSI_D00					= 0x530,										// 0x500[AF5] 0x030[D group] 0x000[pin  0]
	SPI1_MOSI_D04					= 0x134,										// 0x100[AF1] 0x030[D group] 0x004[pin  4]
	SPI1_MOSI_D05					= 0x035,										// 0x000[AF0] 0x030[D group] 0x005[pin  5]

	SPI2_MOSI_A15					= 0x40F,										// 0x400[AF4] 0x000[A group] 0x00F[pin 15]
	SPI2_MOSI_B12					= 0x31C,										// 0x300[AF3] 0x010[B group] 0x00C[pin 12]
	SPI2_MOSI_B13					= 0x41D,										// 0x400[AF4] 0x010[B group] 0x00D[pin 13]
	SPI2_MOSI_B14					= 0x11E,										// 0x100[AF1] 0x010[B group] 0x00E[pin 14]
	SPI2_MOSI_B15					= 0x01F,										// 0x000[AF0] 0x010[B group] 0x00F[pin 15]
	SPI2_MOSI_C03					= 0x123,										// 0x100[AF1] 0x020[C group] 0x003[pin 03]
	SPI2_MOSI_C10					= 0x62A,										// 0x600[AF6] 0x020[C group] 0x00A[pin 10]
	SPI2_MOSI_C11					= 0x32B,										// 0x300[AF3] 0x020[C group] 0x00B[pin 11]
	SPI2_MOSI_C12					= 0x52C,										// 0x500[AF5] 0x020[C group] 0x00C[pin 12]
}SPI_MOSIPIN_enum;

//枚举串口引脚	此枚举定义不允许用户修改
typedef enum
{
	SPI_MISO_NULL					= 0xFFF,										// 占位使用 填入此参数标识不使用 MISO

	SPI1_MISO_A06					= 0x006,										// 0x000[AF0] 0x000[A group] 0x006[pin  6]
	SPI1_MISO_B04					= 0x014,										// 0x000[AF0] 0x010[B group] 0x004[pin  4]
	SPI1_MISO_C05					= 0x625,										// 0x600[AF6] 0x020[C group] 0x005[pin  5]
	SPI1_MISO_D01					= 0x531,										// 0x500[AF5] 0x030[D group] 0x001[pin  1]
	SPI1_MISO_D03					= 0x633,										// 0x600[AF6] 0x030[D group] 0x003[pin  3]
	SPI1_MISO_D04					= 0x034,										// 0x000[AF0] 0x030[D group] 0x004[pin  4]
	SPI1_MISO_D05					= 0x135,										// 0x100[AF1] 0x030[D group] 0x005[pin  5]

	SPI2_MISO_A15					= 0x50F,										// 0x500[AF5] 0x000[A group] 0x00F[pin 15]
	SPI2_MISO_B12					= 0x41C,										// 0x400[AF4] 0x010[B group] 0x00C[pin 12]
	SPI2_MISO_B13					= 0x11D,										// 0x100[AF1] 0x010[B group] 0x00D[pin 13]
	SPI2_MISO_B14					= 0x01E,										// 0x000[AF0] 0x010[B group] 0x00E[pin 14]
	SPI2_MISO_B15					= 0x31F,										// 0x300[AF3] 0x010[B group] 0x00F[pin 15]
	SPI2_MISO_C02					= 0x122,										// 0x100[AF1] 0x020[C group] 0x002[pin  2]
	SPI2_MISO_C10					= 0x32A,										// 0x300[AF3] 0x020[C group] 0x00A[pin 10]
	SPI2_MISO_C11					= 0x62B,										// 0x600[AF6] 0x020[C group] 0x00B[pin 11]
	SPI2_MISO_C12					= 0x42C,										// 0x400[AF4] 0x020[C group] 0x00C[pin 12]
}SPI_MISOPIN_enum;

//枚举串口引脚	此枚举定义不允许用户修改
typedef enum
{
	SPI_NSS_NULL					= 0xFFF,										// 占位使用 填入此参数标识不使用 NSS

	SPI1_NSS_A04					= 0x004,										// 0x000[AF0] 0x000[A group] 0x004[pin  4]
	SPI1_NSS_A05					= 0x105,										// 0x100[AF1] 0x000[A group] 0x005[pin  5]
	SPI1_NSS_A15					= 0x00F,										// 0x000[AF0] 0x000[A group] 0x00F[pin 15]
	SPI1_NSS_C06					= 0x626,										// 0x600[AF6] 0x020[C group] 0x006[pin  6]
	SPI1_NSS_D02					= 0x532,										// 0x500[AF5] 0x030[D group] 0x002[pin  2]

	SPI2_NSS_B09					= 0x519,										// 0x500[AF5] 0x010[B group] 0x009[pin  9]
	SPI2_NSS_B12					= 0x01C,										// 0x000[AF0] 0x010[B group] 0x00C[pin 12]
	SPI2_NSS_B13					= 0x31D,										// 0x300[AF3] 0x010[B group] 0x00D[pin 13]
	SPI2_NSS_B14					= 0x41E,										// 0x400[AF4] 0x010[B group] 0x00E[pin 14]
	SPI2_NSS_B15					= 0x11F,										// 0x100[AF1] 0x010[B group] 0x00F[pin 15]
	SPI2_NSS_C10					= 0x52A,										// 0x500[AF5] 0x020[C group] 0x00A[pin 10]
	SPI2_NSS_C11					= 0x42B,										// 0x400[AF4] 0x020[C group] 0x00B[pin 11]
	SPI2_NSS_C12					= 0x62C,										// 0x600[AF6] 0x020[C group] 0x00C[pin 12]
}SPI_NSSPIN_enum;

extern SPI_TypeDef *spi_index[2];

void	spi_init		(SPIN_enum spi_n, SPI_SCKPIN_enum sck_pin, SPI_MOSIPIN_enum mosi_pin, SPI_MISOPIN_enum miso_pin, SPI_NSSPIN_enum cs_pin, uint8 mode, uint32 baud);
void	spi_mosi		(SPIN_enum spi_n, uint8 *modata, uint8 *midata, uint32 len);

#endif
