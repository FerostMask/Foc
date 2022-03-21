/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
static void driverInit(void);
static void dcOffsetCalibration(struct Sensor *sensor);
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
uint8_t clockwiseRotate[] = {0x04, 0x06, 0x02, 0x03, 0x01, 0x05};     // 100 110 010 011 001 101
uint8_t anticlockwiseRotate[] = {0x05, 0x01, 0x03, 0x02, 0x06, 0x04}; // 101 001 011 010 110 100
Sensor sensor;
Driver driver = {
    .init = driverInit,
    .dutyThreshold = PWM_MAX_DUTY * 0.7f,
    .sensor = &sensor,
};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		   驱动器初始化 	     */
/*==============================*/
static void driverInit(void)
{
    adcInit(SO1_CH); // current sensor1
    adcInit(SO2_CH); // current sensor2
    adcInit(VOL_CH); // voltage sensor

    pwmInit(INH_A, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // A | H
    pwmInit(INH_B, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // B | H
    pwmInit(INH_C, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // C | H
    afioInit(INL_A, AFMODE_AL);                              // AN | L
    afioInit(INL_B, AFMODE_BL);                              // BN | L
    afioInit(INL_C, AFMODE_CL);                              // CN | L

    spiDevice.drv->gainSet(GAIN_SET); // 设置电流检测增益

    dcOffsetCalibration(driver.sensor); // 直流偏移校准
}

static inline int16_t medianFilter(BLDC_ADC_Enum channel) // 中值滤波 | 直流偏移校准辅助函数
{
    int16_t maxIndex = 0, minIndex = 0;
    int16_t val[3] = {0};
    for (int j = 0; j < 3; j++) // 电流采样
    {
        val[j] = adcRead(channel);
    }
    if (val[0] == val[1] == val[2])
    {
        return val[0];
    }
    for (int j = 1; j < 3; j++) // 找中值
    {
        if (val[maxIndex] < val[j])
            maxIndex = j;
        if (val[minIndex] > val[j])
            minIndex = j;
    }
    return val[3 - maxIndex - minIndex]; // 返回中值
}
/*------------------------------*/
/*		   直流偏移校准 	     */
/*==============================*/
static void dcOffsetCalibration(struct Sensor *sensor)
{
    int32_t sum1 = 0, sum2 = 0;
    gpioSetHigh(DC_CAL);                           // 拉高DC_CAL，短路负载方便直流偏移校准
    delay_ms(50);                                  // 等待电压稳定
    for (int i = 0; i < DCCAL_SAMPLING_TIMES; i++) // 中值平均滤波 | 3次采样取中值
    {
        sum1 += medianFilter(SO1_CH);
        sum2 += medianFilter(SO2_CH);
    }
    sensor->offset1 = sum1 / DCCAL_SAMPLING_TIMES;
    sensor->offset2 = sum2 / DCCAL_SAMPLING_TIMES;
    gpioSetLow(DC_CAL); // 重连负载
    delay_ms(50);       // 等待电压稳定
}

void vacSensorRead(void)
{
    int16_t current1 = adcRead(SO1_CH);
    int16_t current2 = adcRead(SO2_CH);
    int16_t voltage = adcRead(VOL_CH);

    // ips114_showuint16(0, 0, current1);
    // ips114_showuint16(0, 1, current2);
    // ips114_showuint16(0, 2, voltage);
}
/*------------------------------*/
/*		    无刷开环控制   	     */
/*==============================*/
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