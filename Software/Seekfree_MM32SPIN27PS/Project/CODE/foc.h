#ifndef _FOC_H
#define _FOC_H

#include "stdint.h"
#include "stdbool.h"
#include "zf_adc.h"
#include "zf_gpio.h"
#include "zf_pwm.h"
#include "zf_systick.h"
#include "slave.h"
#include "driver.h"
/*------------------------------------------------------*/
/* 					  枚举类型定义 						*/
/*======================================================*/
typedef struct Foc
{
    struct Driver *driver;
    struct Sensor *sensor;
    struct Magenc *encoder;
	
    void (*init)(struct Foc *, struct Driver *, struct Magenc *);
	void (*transform)(struct Foc *);

    float currentA;
    float *currentB;
    float *currentC;

    float *voltage;

    float alpha;
    float beta;

    float *angle;

    float cycleGain;

    float Id;
    float Iq;

    float revAlpha;
    float revBeta;
} Foc;
/*------------------------------------------------------*/
/* 					    外部声明 						*/
/*======================================================*/
extern Foc foc;
#endif
