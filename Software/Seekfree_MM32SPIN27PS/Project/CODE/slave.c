/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "funcinter.h"
#include "slave.h"
#include "zf_systick.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "string.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
static void SPIInit(void);
static void readEncoder(void);
static void infoGet(void);
static void gainSet(CSA_GAIN_Enum gain);
static int16_t readDrvRegister(DRV_REGISTER_Enum address);
static bool writeDrvRegister(DRV_REGISTER_Enum address, int16_t sdiData);
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
Magenc encoder = {
	.read = readEncoder,
	.absAngle = 0,
	.rawData = 0,
	.checkMode = false, // 是否进行奇偶校验
};

Drv drv = {
	.info = infoGet,
	.gainSet = gainSet,
};

SPISlave spiDevice = {
	.init = SPIInit,
	.enc = &encoder,
	.drv = &drv,
};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
static void SPIInit(void)
{
	// SPI Initialize
	outputPinInit(SPI_SCK);	 // SCK
	outputPinInit(SPI_MOSI); // MOSI
	outputPinInit(ENC_CS);	 // AS5047P CS
	outputPinInit(DRV_CS);	 // DRV8301 CS
	inputPinInit(SPI_MISO);	 // MISO
	// DRV Initialize
	outputPinInit(DC_CAL);
	outputPinInit(EN_GATE);
	gpioSetLow(DC_CAL);
	gpioSetHigh(EN_GATE);
}

static void readEncoder(void)
{
	uint8_t j = 0;		   // 获取数据计数
	uint8_t clock = 0;	   // 时钟信号
	uint8_t getLevel = 0;  // 获得电平状态
	uint8_t parity = 0;	   // 奇偶校验
	int16_t rawData = 0;   // 获得的原始数据
	gpioSetHigh(SPI_MOSI); // Master output: 0xFFFF
	gpioSetLow(SPI_SCK);
	gpioSetLow(ENC_CS); // 开始通信
	clock = 0;			// 时钟信号从低电平开始
	delay_ns(400);		// tl = 350ns
	for (int i = 0; i < COMN_CYCLE; i++)
	{
		clock = !clock;				  // 时钟跳变
		gpioSetLevel(SPI_SCK, clock); // tclk = 100ns
		if (!clock)					  // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
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
	gpioSetLow(SPI_SCK);
	delay_ns(100);		 // th = tclk/2 = 50ns
	gpioSetHigh(ENC_CS); // 结束通信
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

	ips114_showfloat(0, 2, encoder.absAngle, 3, 2);
	ips114_showuint16(0, 1, encoder.rawData);
}

static void infoGet(void)
{
	char rsShow[17] = {0};
	int16_t result = readDrvRegister(CONTROL_REGISTERS_SECOND);
	for (int i = 0; i < 16; i++)
	{
		if (result & (0x01 << i))
			rsShow[15 - i] = '1';
		else
			rsShow[15 - i] = '0';
	}
	ips114_showstr(0, 4, rsShow);
}

static void gainSet(CSA_GAIN_Enum gain)
{
	int16_t originRegister = readDrvRegister(CONTROL_REGISTERS_SECOND);
	originRegister &= GAIN_BITS;
	originRegister |= gain << BIAS_OF_GAIN;
	bool success = writeDrvRegister(CONTROL_REGISTERS_SECOND, originRegister);
	//	if (success)
	//	{
	//		ips114_showstr(0, 5, "success!");
	//	}
	//	else
	//	{
	//		ips114_showstr(0, 5, "fail!!!!");
	//	}
}

static int16_t readDrvRegister(DRV_REGISTER_Enum address)
{
	uint8_t j = 0;		  // 获取数据计数
	uint8_t clock = 0;	  // 时钟信号
	uint8_t getLevel = 0; // 获得电平状态
	int16_t rawData = 0;  // 获得的原始数据
	int16_t sdiData = 0;
	// 写数据
	sdiData |= (0x01 << DATA_MSB) | (address << ADDRESS_BIT); // Read
	gpioSetLow(SPI_SCK);
	gpioSetLow(DRV_CS); // 开始通信
	clock = 0;			// 时钟信号从低电平开始
	delay_ns(100);		// tSU_SCS = 50ns
	for (int i = 0; i < COMN_CYCLE; i++)
	{
		clock = !clock;				  // 时钟跳变
		gpioSetLevel(SPI_SCK, clock); // tCLKH = 40ns
		if (clock)					  // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
		{
			uint8_t setLevel = (sdiData >> (DATA_MSB - j)) & 0x01; // 上升沿改变输出数据
			gpioSetLevel(SPI_MOSI, setLevel);
			j++;
		}
	}
	gpioSetLow(SPI_SCK);
	delay_ns(100);		 // tHD_SCS = 50ns
	gpioSetHigh(DRV_CS); // 结束通信

	// 读数据
	gpioSetHigh(SPI_MOSI); // Master output: 0xFFFF | fault data
	gpioSetLow(SPI_SCK);
	gpioSetLow(DRV_CS); // 开始通信
	clock = 0;			// 时钟信号从低电平开始
	delay_ns(100);		// tSU_SCS = 50ns
	j = 0;
	for (int i = 0; i < COMN_CYCLE; i++)
	{
		clock = !clock;				  // 时钟跳变
		gpioSetLevel(SPI_SCK, clock); // tCLKH = 40ns
		if (!clock)					  // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
		{
			getLevel = gpio_get(SPI_MISO);		   // 获取电平
			rawData |= getLevel << (DATA_MSB - j); // 记录电平
			j++;
		}
	}
	gpioSetLow(SPI_SCK);
	delay_ns(100);		 // tHD_SCS = 50ns
	gpioSetHigh(DRV_CS); // 结束通信
	// ips114_showint16(0, 5, rawData);
	return rawData;
}

static bool writeDrvRegister(DRV_REGISTER_Enum address, int16_t sdiData)
{
	uint8_t j = 0;		  // 获取数据计数
	uint8_t clock = 0;	  // 时钟信号
	uint8_t getLevel = 0; // 获得电平状态
	int16_t rawData = 0;  // 获得的原始数据
	sdiData &= 0x03FF;
	// 写数据
	sdiData |= (0x00 << DATA_MSB) | (address << ADDRESS_BIT); // Write
	gpioSetLow(SPI_SCK);
	gpioSetLow(DRV_CS); // 开始通信
	clock = 0;			// 时钟信号从低电平开始
	delay_ns(100);		// tSU_SCS = 50ns
	for (int i = 0; i < COMN_CYCLE; i++)
	{
		clock = !clock;				  // 时钟跳变
		gpioSetLevel(SPI_SCK, clock); // tCLKH = 40ns
		if (clock)					  // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
		{
			uint8_t setLevel = (sdiData >> (DATA_MSB - j)) & 0x01; // 上升沿改变输出数据
			gpioSetLevel(SPI_MOSI, setLevel);
			j++;
		}
	}
	gpioSetLow(SPI_SCK);
	delay_ns(100);		 // tHD_SCS = 50ns
	gpioSetHigh(DRV_CS); // 结束通信

	// 读数据
	gpioSetHigh(SPI_MOSI); // Master output: 0xFFFF | fault data
	gpioSetLow(SPI_SCK);
	gpioSetLow(DRV_CS); // 开始通信
	clock = 0;			// 时钟信号从低电平开始
	delay_ns(100);		// tSU_SCS = 50ns
	j = 0;
	for (int i = 0; i < COMN_CYCLE; i++)
	{
		clock = !clock;				  // 时钟跳变
		gpioSetLevel(SPI_SCK, clock); // tCLKH = 40ns
		if (!clock)					  // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
		{
			getLevel = gpio_get(SPI_MISO);		   // 获取电平
			rawData |= getLevel << (DATA_MSB - j); // 记录电平
			j++;
		}
	}
	gpioSetLow(SPI_SCK);
	delay_ns(100);		 // tHD_SCS = 50ns
	gpioSetHigh(DRV_CS); // 结束通信
	if (!((rawData >> DATA_MSB) & 0x01))
	{
		return true;
	}
	return false;
}