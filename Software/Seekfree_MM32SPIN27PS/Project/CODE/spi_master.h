#ifndef _SPI_MASTER_
#define _SPI_MASTER_
#include "zf_spi.h"
/*------------------------------------------------------*/
/* 					  枚举类型定义 						*/
/*======================================================*/
//	SPI相关
typedef enum{
	SPI_N = SPI_1,//SPI模块
	SPI_SCLK = SPI1_SCK_A04,//时钟引脚
	SPI_MOSI = SPI1_MOSI_A07,//主发从收引脚
	SPI_MISO = SPI_MISO_NULL,//主收从发引脚
	SPI_CS = SPI1_NSS_A04,//片选引脚
	SPI_MODE = 0,//SPI通信模式
	SPI_BAUD = 1 * 1000 * 1000,//时钟频率
	SPI_CTN = 1,//是否持续保持有效状态（CS持续拉低）
}SPI_M_enum;
//	通信相关
typedef enum{
	INFO_LEN = 4,//数据信息头长度（指令位+地址寄存器+通信指示+编号）
	DATA_START = 2,//发送数据起始位（除去指令位+地址寄存器）
	INSTRUCTION_POSITION = 0,//通信指示位置
	NUMBER_POSITION = 1,//编号位置
	DATA_POSITION = 2,//数据位置
	MAX_DATA_LEN = 30,//一次发送最长数据
	ORI_ARRAY_LEN = MAX_DATA_LEN + INFO_LEN,
	
	READ_REGISTER = 0x04,//指令位 | 读状态寄存器
	PLACEHOLDER = 0x00,//占位
	SEND_DATA_ONLY = 0x02,//指令位 | 只发送数据
	SEND_DATA_REGISTER = 0x00,//寄存器
	
	SPI_COMN_START = 0xA0,
	SPI_COMN_UNDERWAY = 0xA1,
	SPI_COMN_END = 0xA3,
	
	WIFI_SET_NAME = 0xC0,
	WIFI_SET_PASSWORD = 0xC1,
	WIFI_SET_IP = 0xC2,
	WIFI_SET_SENDPORT = 0xC3,
	WIFI_SET_RECEIVEPORT = 0xC4,
	
	WIFI_INITIALIZATION = 0xD0,
	WIFI_INIT_FAIL = 0x00,
	WIFI_INIT_SUCCESS = 0x01,
}COMN_enum;
/*------------------------------------------------------*/
/* 					   结构体定义 						*/
/*======================================================*/
//	SPI相关
typedef struct spi_m{
	void (*init)(void);//主设备初始化函数
	char *spi_sd;//数据发送数组
	char *instruction;
	char *number;
}spi_m;
//	WiFi相关
typedef struct wifi_info{
	char *name;
	char *password;
	char *ip;
	char *send_port;
	char *receive_port;
}wifi_info;
/*------------------------------------------------------*/
/* 					   宏定义函数 						*/
/*======================================================*/
#define spi_send_data(modata, len) spi_mosi(SPI_N, modata, NULL, len)
#define spi_sar_data(modata, midata, len) spi_mosi(SPI_N, modata, midata, len)
/*------------------------------------------------------*/
/* 					  外部变量声明 						*/
/*======================================================*/

#endif
