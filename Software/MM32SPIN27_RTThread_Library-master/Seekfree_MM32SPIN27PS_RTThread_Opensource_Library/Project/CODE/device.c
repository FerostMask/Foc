/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "device.h"
#include "funcinter.h"
#include "zf_systick.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "string.h"
#include "scope.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
enum DEVICE_enum
{
    SPI_SCK = B3,  // 时钟脚
    SPI_MOSI = B5, // 主发从收
    SPI_MISO = B4, // 主收从发
    ENC_CS = A15,  // 编码器片选引脚
    DRV_CS = C6,   // DRV8301SPI片选引脚

    ADDRESS_BIT = 11, // 地址位
    UMax14 = 16384 - 1,
    CIRCLE = 360,

    COMN_CYCLE = 32, // 通信周期
    DATA_MSB = 15,   // 数据最高位
    ERRORFRAME = 14, // 指令校验位

    DC_CAL = D3,
    EN_GATE = A12,
};
/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/
void readEncoder(void);
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
uint8_t *readAngle = {0xFF, 0xFF};
Magenc encoder = {
    .read = readEncoder,
    .absAngle = 10,
    .biasAngle = 0,
    .rawData = 0,
    .checkMode = false, // 是否进行奇偶校验
};

// Drv drv = {
//     .info = infoGet,
//     .gainSet = gainSet,
// };
/*------------------------------------------------------*/
/*                       函数定义                       */
/*======================================================*/
void deviceInit(void)
{
    // SPI Initialize
    outputPinInit(SPI_SCK);  // SCK
    outputPinInit(SPI_MOSI); // MOSI
    outputPinInit(ENC_CS);   // AS5047P CS
    outputPinInit(DRV_CS);   // DRV8301 CS
    inputPinInit(SPI_MISO);  // MISO
    // DRV Initialize
    outputPinInit(DC_CAL);
    outputPinInit(EN_GATE);
    gpioSetLow(DC_CAL);
    gpioSetHigh(EN_GATE);
    // push data into scope
    scopePushValue(&encoder.absAngle, sizeof(encoder.absAngle), "angle", false);
}

static void readEncoder(void)
{
    uint8_t j = 0;         // 获取数据计数
    uint8_t clock = 0;     // 时钟信号
    uint8_t getLevel = 0;  // 获得电平状态
    uint8_t parity = 0;    // 奇偶校验
    int16_t rawData = 0;   // 获得的原始数据
    gpioSetHigh(SPI_MOSI); // Master output: 0xFFFF
    gpioSetLow(SPI_SCK);
    gpioSetLow(ENC_CS); // 开始通信
    clock = 0;          // 时钟信号从低电平开始
    delay_ns(400);      // tl = 350ns
    for (int i = 0; i < COMN_CYCLE; i++)
    {
        clock = !clock;               // 时钟跳变
        gpioSetLevel(SPI_SCK, clock); // tclk = 100ns
        if (!clock)                   // MODE : CPOL = 0, CPHA = 1 | 低电平休息，第二个跳变沿（下降沿）采样
        {
            getLevel = gpio_get(SPI_MISO);         // 获取电平
            rawData |= getLevel << (DATA_MSB - j); // 记录电平
            if (getLevel)
            {
                parity++; // 奇偶校验
            }
            j++;
        }
    }
    gpioSetLow(SPI_SCK);
    delay_ns(80);        // th = tclk/2 = 50ns
    gpioSetHigh(ENC_CS); // 结束通信
    if (rawData & (0x01 << ERRORFRAME))
    { // 数据校验
        return;
    }
    if (encoder.checkMode) // 奇偶校验
    {
        if (rawData & 0x80)
        {             // 奇数个1
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
    encoder.rawData = rawData & 0x3FFF;                                          // 获取原始数据
    encoder.absAngle = ((float)encoder.rawData / (float)UMax14) * (float)CIRCLE; // 转换绝对角度（角度制）
    encoder.absAngle = encoder.absAngle + encoder.biasAngle;                     // 加上偏移角度
    encoder.absAngle = fmod(encoder.absAngle, 360);

    // ips114_showfloat(0, 2, encoder.absAngle, 3, 2);
    // ips114_showuint16(0, 1, encoder.rawData);
}
