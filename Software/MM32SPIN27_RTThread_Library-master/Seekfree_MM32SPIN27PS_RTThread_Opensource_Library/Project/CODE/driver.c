/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "driver.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/

/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/
static void cycleRotate(struct Driver *driver, int8_t cycles, int32_t duty, DIRECTION_Enum dir);
static void driverInit(void);
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
uint8_t clockwiseRotate[] = {0x04, 0x06, 0x02, 0x03, 0x01, 0x05, 0x04};     // 100 110 010 011 001 101
uint8_t anticlockwiseRotate[] = {0x05, 0x01, 0x03, 0x02, 0x06, 0x04}; // 101 001 011 010 110 100

Motor motor = {
    .cycleRotate = cycleRotate,
};

Driver driver = {
    // .sensor = &sensor,
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
static void driverInit(void)
{
    // adcInit(SO1_CH); // current sensor1
    // adcInit(SO2_CH); // current sensor2
    // adcInit(VOL_CH); // voltage sensor

    pwmInit(INH_A, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // A | H
    pwmInit(INH_B, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // B | H
    pwmInit(INH_C, PWM_DEFAULT_FREQUENCY, PWM_DEFAULT_DUTY); // C | H
    afioInit(INL_A, AFMODE_AL);                              // AN | L
    afioInit(INL_B, AFMODE_BL);                              // BN | L
    afioInit(INL_C, AFMODE_CL);                              // CN | L

    // spiDevice.drv->gainSet(GAIN_SET); // 设置电流检测增益

    // dcOffsetCalibration(driver.sensor); // 直流偏移校准

    // sensor.ratio = sensor.vRef / ADC_UMAXW;          // ADC采样转换系数
    // sensor.shuntResistor = 1 / sensor.shuntResistor; // 分流电阻倒数
    // sensor.gain = gainValue[GAIN_SET];               // 增益系数
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
