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