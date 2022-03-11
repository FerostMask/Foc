#ifndef _MAGENC_H
#define _MAGENC_H
#include "zf_spi.h"
/*------------------------------------------------------*/
/* 					  枚举类型定义 						*/
/*======================================================*/
typedef enum
{
    SPI_N = SPI_1,              // SPI模块
    SPI_SCLK = SPI1_SCK_B03,    //时钟引脚
    SPI_MOSI = SPI1_MOSI_B05,   //主发从收引脚
    SPI_MISO = SPI1_MISO_B04,   //主收从发引脚
    SPI_CS = SPI1_NSS_A15,      //片选引脚
    SPI_MODE = 1,               // SPI通信模式
    SPI_BAUD = 1 * 1000 * 1000, //时钟频率
    SPI_CTN = 1,                //是否持续保持有效状态（CS持续拉低）
} SPI_M_enum;
/*------------------------------------------------------*/
/* 					   宏定义函数 						*/
/*======================================================*/
#define spi_send_data(modata, len) spi_mosi(SPI_N, modata, NULL, len)
#define spi_sar_data(modata, midata, len) spi_mosi(SPI_N, modata, midata, len)

void encoderInit(void);
void readEncoder(void);

#endif
