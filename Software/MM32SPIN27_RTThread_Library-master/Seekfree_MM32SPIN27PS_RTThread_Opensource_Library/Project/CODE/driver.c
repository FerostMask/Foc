/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "driver.h"
#include "funcinter.h"
#include "scope.h"
#include "SEEKFREE_IPS114_SPI.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
enum BLDC_ADC_Enum
{
    ADC_N = ADC_1,       // ADC模块
    ADC_RES = ADC_12BIT, // ADC分辨率

    ADC_UMAXW = 4096 - 1, // ADC精度无符号数对应的最大值

    SO1_CH = ADC1_CH10_B02, // SO1的ADC引脚
    SO2_CH = ADC1_CH09_B01, // SO2的ADC引脚
    VOL_CH = ADC1_CH08_B00, // 电压ADC引脚

    DCCAL_SAMPLING_TIMES = 30,
};

enum DRIVER_INFO_Enum
{
    GAIN_SET = GAIN_10VPERV,
};
/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/
static void cycleRotate(struct Driver *driver, int8_t cycles, int32_t duty, DIRECTION_Enum dir);
static void dcOffsetCalibration(struct Sensor *sensor);
static void sampling(struct Sensor *sensor);
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
uint8_t clockwiseRotate[] = {0x04, 0x06, 0x02, 0x03, 0x01, 0x05, 0x04}; // 100 110 010 011 001 101
uint8_t anticlockwiseRotate[] = {0x05, 0x01, 0x03, 0x02, 0x06, 0x04};   // 101 001 011 010 110 100
int8_t gainValue[] = {GAIN_VALUE_10VPERV, GAIN_VALUE_20VPERV, GAIN_VALUE_40VPERV, GAIN_VALUE_80VPERV};

Sensor sensor = {
    .sampling = sampling,
    .calibration = dcOffsetCalibration,

    .current1 = 0,
    .current2 = 0,

    .voltage = 0,
    .volRatio = 4,

    .shuntResistor = 0.01f,
    .vRef = 3.3,
    .gain = 0,
    .ratio = 0,

    .offset1 = ADC_UMAXW >> 1,
    .offset2 = ADC_UMAXW >> 1,
};

Motor motor = {
    .cycleRotate = cycleRotate,
};

Driver driver = {
    .sensor = &sensor,
    .motor = &motor,
    .init = driverInit,
    .dutyThreshold = PWM_MAX_DUTY * 0.7f,
};
/*------------------------------------------------------*/
/*                       函数定义                       */
/*======================================================*/
/*------------------------------*/
/*		   驱动器初始化 	     */
/*==============================*/
void driverInit(void)
{
    //? ADC initialization
    adcInit(SO1_CH); // current sensor1
    adcInit(SO2_CH); // current sensor2
    adcInit(VOL_CH); // voltage sensor
    //? PWM output initialization
    pwmInit(INH_A, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // A | H
    pwmInit(INH_B, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // B | H
    pwmInit(INH_C, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // C | H
    afioInit(INL_A, AFMODE_AL);                              // AN | L
    afioInit(INL_B, AFMODE_BL);                              // BN | L
    afioInit(INL_C, AFMODE_CL);                              // CN | L

    //? DRV set current shunt amplifiers GAIN
    delay_ms(10);                     // 等待10ms，等之前的任务处理完
    spiDevice.drv->gainSet(GAIN_SET); // 设置电流检测增益

    sensor.ratio = sensor.vRef / ADC_UMAXW;          // ADC采样转换系数
    sensor.shuntResistor = 1 / sensor.shuntResistor; // 分流电阻倒数
    sensor.gain = gainValue[GAIN_SET];               // 增益系数

    delay_ms(10);                              // 等待增益系数调整后电压稳定
    driver.sensor->calibration(driver.sensor); // 直流偏移校准
}

/*------------------------------*/
/*		   直流偏移校准 	     */
/*==============================*/
static void dcOffsetCalibration(struct Sensor *sensor)
{
    int32_t sum1 = 0, sum2 = 0;

    for (int i = 0; i < DCCAL_SAMPLING_TIMES; i++) // 平均滤波
    {
        sum1 += adcRead(SO1_CH);
        sum2 += adcRead(SO2_CH);
        delay_ms(1);
    }
    sensor->offset1 = sum1 / DCCAL_SAMPLING_TIMES;
    sensor->offset2 = sum2 / DCCAL_SAMPLING_TIMES;

    // ips114_showint16(0, 1, sensor->offset1);
    // ips114_showint16(0, 2, sensor->offset2);
}

/*------------------------------*/
/*		   电压电流采样 	     */
/*==============================*/
static void sampling(struct Sensor *sensor)
{
    int16_t currentSample1 = adcRead(SO1_CH);
    int16_t currentSample2 = adcRead(SO2_CH);

    int16_t voltageSample = adcRead(VOL_CH);

    sensor->current1 = ((sensor->offset1 - (float)currentSample1) / sensor->gain) * sensor->ratio * sensor->shuntResistor; // init: sensor.shuntResistor = 1/sensor.shuntResistor
    sensor->current2 = ((sensor->offset2 - (float)currentSample2) / sensor->gain) * sensor->ratio * sensor->shuntResistor;

    sensor->voltage = voltageSample * sensor->ratio * sensor->volRatio; // 串联分压

    //? 调试代码
    // ips114_showfloat(0, 0, sensor->current1, 3, 3);
    // ips114_showfloat(0, 1, sensor->current2, 3, 3);
    //	ips114_showfloat(0, 2, sensor->voltage, 3, 3);
}

/*------------------------------*/
/*		    无刷开环控制   	     */
/*==============================*/
static void cycleRotate(struct Driver *driver, int8_t cycles, int32_t duty, DIRECTION_Enum dir) // 限制可转圈数在0~127
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
