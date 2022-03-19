/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
uint8_t clockwiseRotate[] = {0x04, 0x06, 0x02, 0x03, 0x01, 0x05};     // 100 110 010 011 001 101
uint8_t anticlockwiseRotate[] = {0x05, 0x01, 0x03, 0x02, 0x06, 0x04}; // 101 001 011 010 110 100
Driver driver = {
    .dutyThreshold = PWM_MAX_DUTY * 0.7f,
};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
void vacSensorInit(void)
{
    adcInit(SO1_CH); // current sensor1
    adcInit(SO2_CH); // current sensor2
    adcInit(VOL_CH); // voltage sensor
}

void vacSensorRead(void)
{
    uint16_t current1 = adcRead(SO1_CH);
    uint16_t current2 = adcRead(SO2_CH);
    uint16_t voltage = adcRead(VOL_CH);
    ips114_showuint16(0, 0, current1);
    ips114_showuint16(0, 1, current2);
    ips114_showuint16(0, 2, voltage);
}

void driverInit(void)
{
    pwmInit(INH_A, PWM_DEFAULT_FREQUENCY, 0); // A | H
    pwmInit(INH_B, PWM_DEFAULT_FREQUENCY, 0); // B | H
    pwmInit(INH_C, PWM_DEFAULT_FREQUENCY, 0); // C | H
    afioInit(INL_A, AFMODE_AL);               // AN | L
    afioInit(INL_B, AFMODE_BL);               // BN | L
    afioInit(INL_C, AFMODE_CL);               // CN | L
}

void cycleRotate(struct Driver *driver, int8_t cycles, int32_t duty, DIRECTION_Enum dir) // 限制可转圈数在0~127
{
    // 限制duty范围
    if (duty > driver->dutyThreshold)
    {
        duty = driver->dutyThreshold;
    }
    if (duty < 0)
    {
        duty = 0;
    }
    // 旋转电机
    switch (dir)
    {
    case CLOCKWISE:
        for (int i = 0; i < cycles; i++)
        {
            for (int j = 0; j < sizeof(clockwiseRotate); j++)
            {
                if (clockwiseRotate[j] & (0x01 << BITS_A)) // A
                {
                    dutyUpdate(INH_A, duty);
                }
                else
                {
                    dutyUpdate(INH_A, 0);
                }
                if (clockwiseRotate[j] & (0x01 << BITS_B)) // B
                {
                    dutyUpdate(INH_B, duty);
                }
                else
                {
                    dutyUpdate(INH_B, 0);
                }
                if (clockwiseRotate[j] & (0x01 << BITS_C)) // C
                {
                    dutyUpdate(INH_C, duty);
                }
                else
                {
                    dutyUpdate(INH_C, 0);
                }
                delay_ms(DELAY_FOR_SPEED);
            }
        }
        break;
    case ANTICLOCKWISE:
        for (int i = 0; i < cycles; i++)
        {
            for (int j = 0; j < sizeof(anticlockwiseRotate); j++)
            {
                if (anticlockwiseRotate[j] & (0x01 << BITS_A)) // A
                {
                    dutyUpdate(INH_A, duty);
                }
                else
                {
                    dutyUpdate(INH_A, 0);
                }
                if (anticlockwiseRotate[j] & (0x01 << BITS_B)) // B
                {
                    dutyUpdate(INH_B, duty);
                }
                else
                {
                    dutyUpdate(INH_B, 0);
                }
                if (anticlockwiseRotate[j] & (0x01 << BITS_C)) // C
                {
                    dutyUpdate(INH_C, duty);
                }
                else
                {
                    dutyUpdate(INH_C, 0);
                }
                delay_ms(DELAY_FOR_SPEED);
            }
        }
        break;
    }
    dutyUpdate(INH_A, 0);
    dutyUpdate(INH_B, 0);
    dutyUpdate(INH_C, 0);
}