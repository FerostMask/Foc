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
    SPI_CS = A15,  // 片选引脚
	
	HIGH_LEVEL = 1,
	LOW_LEVEL = 0,

    UMax14 = 16384 - 1,
	CIRCLE = 360,

    DEFAULT_LEVEL = 1, // 初始电平
    COMN_CYCLE = 32,   // 通信周期
    DATA_MSB = 15,     // 数据最高位
} Magenc_enum;

typedef struct Magenc
{
    void (*init)(void); // 初始化
    void (*read)(void); // 读取编码器数据
    float absAngle;     // 绝对角度
    int16_t rawData;    // 未处理生数据
    bool checkMode;     // 是否开启校验
} Magenc;

/*------------------------------------------------------*/
/* 					    外部声明 						*/
/*======================================================*/
extern Magenc encoder;

#endif
