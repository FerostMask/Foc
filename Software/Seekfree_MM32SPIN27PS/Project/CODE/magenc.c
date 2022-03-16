/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "magenc.h"
#include "zf_systick.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "string.h"
/*--------------------------------------------------------------*/
/* 							宏定义函数 							*/
/*==============================================================*/
#define outputPinInit(pin) gpio_init(pin, GPO, DEFAULT_LEVEL, GPO_PUSH_PULL)  // 输出引脚初始化 | 推挽输出
#define inputPinInit(pin) gpio_init(pin, GPI, DEFAULT_LEVEL, GPI_FLOATING_IN) // 输入引脚初始化 | 浮空输入
#define delay_ns(time) systick_delay_ns(time)								  // ns级延时
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
static void encoderInit(void);
static void readEncoder(void);
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
Magenc encoder = {
	.init = encoderInit,
	.read = readEncoder,
	.absAngle = 0,
	.rawData = 0,
	.checkMode = false,
};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
void encoderInit(void)
{
	outputPinInit(SPI_SCK);	 // SCK
	outputPinInit(SPI_MOSI); // MOSI
	outputPinInit(SPI_CS);	 // CS
	inputPinInit(SPI_MISO);	 // MISO
}

void readEncoder(void)
{
	uint8_t j = 0;		  // 获取数据计数
	uint8_t clock = 0;	  // 时钟信号
	uint8_t getLevel = 0; // 获得电平状态
	uint8_t parity = 0;	  // 奇偶校验
	int16_t rawData = 0;  // 获得的原始数据
	gpio_set(SPI_SCK, 0);
	gpio_set(SPI_CS, 0); // 开始通信
	clock = 0;			 // 时钟信号从低电平开始
	delay_ns(400);		 // tl = 350ns
	for (int i = 0; i < COMN_CYCLE; i++)
	{
		clock = !clock;			  // 时钟跳变
		gpio_set(SPI_SCK, clock); // tclk = 100ns
		if (!clock)				  // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
		{
			getLevel = gpio_get(SPI_MISO);		   // 获取电平
			rawData |= getLevel << (DATA_MSB - j); // 记录电平
			if (getLevel)
			{
				parity++; // 奇偶校验
			}
			j++;
		}
	}
	gpio_set(SPI_SCK, 0);
	delay_ns(100);		 // th = tclk/2 = 50ns
	gpio_set(SPI_CS, 1); // 结束通信
	if (rawData & 0x40)
	{ // 数据校验
		return;
	}
	if (encoder.checkMode) // 奇偶校验
	{
		if (rawData & 0x80)
		{			  // 奇数个1
			parity--; // 减去最高位的1
			if (!(parity & 0x01))
			{
				return;
			}
		}
		else
		{ // 偶数个1
			if (parity & 0x01)
			{
				return;
			}
		}
	}
	encoder.rawData = rawData & 0x3FFF;											 // 获取原始数据
	encoder.absAngle = ((float)encoder.rawData / (float)UMax14) * (float)CIRCLE; // 转换绝对角度（角度制）
																				 // ips114_showfloat(0, 2, encoder.absAngle, 3, 2);
																				 // ips114_showuint16(0, 1, encoder.rawData);
}
