#ifndef _MAGENC_H
#define _MAGENC_H
#include "zf_gpio.h"
#include "stdint.h"
#include "stdbool.h"
/*------------------------------------------------------*/
/* 					  枚举类型定义 						*/
/*======================================================*/
typedef enum
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

    DC_CAL = D3,
    EN_GATE = A12,
} DEVICE_enum;

typedef enum
{
    STATUS_REGISTERS_FIRST = 0x00,
    STATUS_REGISTERS_SECOND = 0x01,
    CONTROL_REGISTERS_FIRST = 0x02,
    CONTROL_REGISTERS_SECOND = 0x03,
} DRV_REGISTER_Enum;

typedef struct Magenc
{
    void (*read)(void); // 读取编码器数据
    float absAngle;     // 绝对角度
    int16_t rawData;    // 未处理生数据
    bool checkMode;     // 是否开启校验
} Magenc;

typedef struct Drv
{
    void (*info)(void); // 读取寄存器信息
} Drv;

typedef struct SPISlave
{
    void (*init)(void); // 初始化
    struct Magenc *enc; // 编码器
    struct Drv *drv;
} SPISlave;
/*------------------------------------------------------*/
/* 					    外部声明 						*/
/*======================================================*/
extern SPISlave spiDevice;
extern Magenc encoder;

#endif