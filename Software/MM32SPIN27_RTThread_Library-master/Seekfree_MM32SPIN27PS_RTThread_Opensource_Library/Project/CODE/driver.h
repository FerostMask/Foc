#ifndef _DRIVER_H
#define _DRIVER_H
#include "stdint.h"
#include "stdbool.h"
#include "zf_adc.h"
#include "zf_gpio.h"
#include "zf_pwm.h"
#include "zf_systick.h"
#include "device.h"
/*------------------------------------------------------*/
/* 					  枚举类型定义 						*/
/*======================================================*/
typedef enum DIRECTION_Enum
{
    CLOCKWISE = 0,     // 顺时针旋转
    ANTICLOCKWISE = 1, // 逆时针旋转
} DIRECTION_Enum;

typedef enum GAIN_VALUE_Enum
{
    GAIN_VALUE_10VPERV = 10,
    GAIN_VALUE_20VPERV = 20,
    GAIN_VALUE_40VPERV = 40,
    GAIN_VALUE_80VPERV = 80,
} GAIN_VALUE_Enum;

enum BLDC_PWM_Enum
{
    BITS_A = 2, // A相占位
    BITS_B = 1, // B相占位
    BITS_C = 0, // C相占位

    TIM_N = TIM_1,         // 定时器模块
    INH_A = TIM_1_CH2_A01, // A相 | 高侧输入
    INL_A = A2,            // A相 | 低侧输入
    INH_B = TIM_1_CH3_A03, // B相 | 高侧输入
    INL_B = A4,            // B相 | 低侧输入
    INH_C = TIM_1_CH1_A08, // C相 | 高侧输入
    INL_C = A7,            // C相 | 低侧输入

    AFMODE_AL = GPIO_AF3, // A相 | 引脚复用模式
    AFMODE_BL = GPIO_AF3, // B相 | 引脚复用模式
    AFMODE_CL = GPIO_AF2, // C相 | 引脚复用模式

    IO_AF_MODE = GPO_AF_PUSH_PUL, // 复用IO模式

    PWM_DEFAULT_FREQUENCY = 20000, // 默认频率 |
    PWM_DEFAULT_DUTY = 0,          // 默认占空比
    PWM_MAX_DUTY = PWM_DUTY_MAX,   // 最大占空比
    DELAY_FOR_SPEED = 10,          // 延时控速 | ms
};

typedef struct Sensor
{
    void (*sampling)(struct Sensor *);
    void (*calibration)(struct Sensor *);

    float current1; // 采样电流1 | 计算值
    float current2; // 采样电流2 | 计算值

    float voltage;  // 采样电压
    float volRatio; // 采样电压计算系数

    float shuntResistor;
    const float vRef; // 参考电压
    float gain;       // 电流检测增益
    float ratio;      // 每伏特电压对应ADC值

    int16_t offset1; // 电流偏置1 | Vref/2
    int16_t offset2; // 电流偏置2 | Vref/2
} Sensor;

typedef struct Motor
{
    void (*cycleRotate)(struct Driver *driver, int8_t cycles, int32_t duty, DIRECTION_Enum dir);
} Motor;

typedef struct Driver
{
    struct Sensor *sensor;
    struct Motor *motor;
    void (*init)(void);
    int32_t dutyThreshold;
} Driver;
/*------------------------------------------------------*/
/* 					    外部声明 						*/
/*======================================================*/
extern Driver driver;
void driverInit(void);
#endif