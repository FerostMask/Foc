#ifndef _DEVICE_H
#define _DEVICE_H
#include "zf_gpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "zf_spi.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
typedef enum CSA_GAIN_Enum
{
    GAIN_10VPERV = 0x00, // 00
    GAIN_20VPERV = 0x01, // 01
    GAIN_40VPERV = 0x02, // 10
    GAIN_80VPERV = 0x03, // 11
} CSA_GAIN_Enum;

typedef enum DRV_REGISTER_Enum
{
    STATUS_REGISTERS_FIRST = 0x00,
    STATUS_REGISTERS_SECOND = 0x01,
    CONTROL_REGISTERS_FIRST = 0x02,
    CONTROL_REGISTERS_SECOND = 0x03,
} DRV_REGISTER_Enum;

typedef enum CSAMC_FUNCTION_Enum
{
    GAIN_BITS = ~0x0C, // Address: CONTROL_REGISTERS_SECOND | BITS: D3 D2 (D1) (D0)
    BIAS_OF_GAIN = 2,
} CSAMC_FUNCTION_Enum;

typedef struct Magenc
{
    void (*read)(void); // 读取编码器数据
    float absAngle;     // 绝对角度
    float biasAngle;    // 偏移角度
    int16_t rawData;    // 未处理生数据
    bool checkMode;     // 是否开启校验
} Magenc;

typedef struct Drv
{
    void (*info)(DRV_REGISTER_Enum); // 读取寄存器信息
    void (*gainSet)(CSA_GAIN_Enum);  // 设置电流检测增益
} Drv;

typedef struct SPISlave
{
    void (*init)(void); // 初始化
    struct Magenc *enc; // 编码器
    struct Drv *drv;
} SPISlave;
/*------------------------------------------------------*/
/*                       外部声明                       */
/*======================================================*/
void deviceInit(void);
extern SPISlave spiDevice;
#endif
