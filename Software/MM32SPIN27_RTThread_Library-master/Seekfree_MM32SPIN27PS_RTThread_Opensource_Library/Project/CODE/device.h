#ifndef _DEVICE_H
#define _DEVICE_H
#include "zf_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "zf_spi.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
typedef enum DEVICE_enum
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
} DEVICE_enum;

typedef struct Magenc
{
    void (*read)(void); // 读取编码器数据
    float absAngle;     // 绝对角度
    float biasAngle;    // 偏移角度
    int16_t rawData;    // 未处理生数据
    bool checkMode;     // 是否开启校验
} Magenc;
/*------------------------------------------------------*/
/*                       外部声明                       */
/*======================================================*/
void deviceInit(void);
extern Magenc encoder;
#endif
