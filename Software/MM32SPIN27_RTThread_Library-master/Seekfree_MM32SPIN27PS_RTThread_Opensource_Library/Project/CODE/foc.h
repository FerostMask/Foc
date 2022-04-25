#ifndef _FOC_H
#define _FOC_H

#include "stdint.h"
#include "stdbool.h"
#include "zf_adc.h"
#include "zf_gpio.h"
#include "zf_pwm.h"
#include "zf_systick.h"
#include "device.h"
#include "driver.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
typedef enum SECTION_Enum // SVPWM对应扇区
{
    I = 0,
    II = 60,
    III = 120,
    IV = 180,
    V = 240,
    VI = 300,
} SECTION_Enum;

typedef struct Foc
{
    struct Driver *driver;
    struct Sensor *sensor;
    struct Magenc *encoder;

    void (*init)(struct Foc *, struct Driver *, struct Magenc *);
    void (*transform)(struct Foc *);
    void (*svpwmAction)(struct Foc *);

    float targetCurrent; // 目标电流大小
    float targetAngle;   // 目标转子角度

    float currentA;  // A相电流
    float *currentB; // B相电流 | 由传感器采集
    float *currentC; // C相电流 | 由传感器采集

    float *voltage; // 电池电压

    float alpha; // Clark变换获得的alpha值
    float beta;  // Clark变换获得的beta值

    float *angle; // 转子角度 | 由磁编码器采集

    float cycleGain; // 周期增益 | 根据电机极数不同设置不同的值

    float Id; // Park变换获得的Id | 目标值为0
    float Iq; // Park变换获得的Iq

    float *commandId;
    float *commandIq;

    float commandAlpha; // 反Park变换获得的alpha值
    float commandBeta;  // 反Park变换获得的beta值

    float Uref;
    float UrefAngle;

    SECTION_Enum section;
} Foc;

// typedef struct pidpara
// {
//     float *act; // 实际值

//     float alpha; // 一阶低通滤波系数
//     float Kp;
//     float Ki;
//     float Kd;
//     //	计算相关
//     float I;          // 积分项暂存
//     float e1, e2, e3; // 误差
//     float rs;         // 计算结果
//     float thrsod;     // 阈值
// } pidpara;
/*------------------------------------------------------*/
/*                       外部声明                       */
/*======================================================*/
extern Foc foc;
#endif