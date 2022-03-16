#include "motor.h"

#define DIR_L				A11
#define DIR_R				C9

#define PWM_TIM				TIM_2
#define PWM_L				TIM_2_CH2_A12
#define PWM_R				TIM_2_CH4_B11

void motor_init(void)
{
    // 请注意 这里接线时请务必不要接错 A11/12一组 B10/11一组 分别控制左右电机
	// 如果是使用杜邦线接 请务必参照说明书接线
	gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);								// GPIO 初始化为输出 默认上拉输出高
	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);								// GPIO 初始化为输出 默认上拉输出高

	pwm_init(PWM_TIM, PWM_L, 10000, 0);												// PWM 通道2 初始化频率10KHz 占空比初始为0
	pwm_init(PWM_TIM, PWM_R, 10000, 0);												// PWM 通道4 初始化频率10KHz 占空比初始为0
}

void motor_pid(int16 expect_speed)
{
    
}


void motor_control(int32 duty_l, int32 duty_r)
{
    //对占空比限幅
	duty_l = ((duty_l>PWM_DUTY_MAX)?(PWM_DUTY_MAX):((duty_l<-PWM_DUTY_MAX)?(-PWM_DUTY_MAX):(duty_l)));
	duty_r = ((duty_r>PWM_DUTY_MAX)?(PWM_DUTY_MAX):((duty_r<-PWM_DUTY_MAX)?(-PWM_DUTY_MAX):(duty_r)));
    
    if(duty_l >= 0)											// 左侧正转
    {
        gpio_set(DIR_L, GPIO_HIGH);							// DIR输出高电平
        pwm_duty_updata(PWM_TIM, PWM_L, duty_l);		    // 计算占空比
    }
    else													// 左侧反转
    {
        gpio_set(DIR_L, GPIO_LOW);							// DIR输出低电平
        pwm_duty_updata(PWM_TIM, PWM_L, -duty_l);			// 计算占空比
    }
    
    if(duty_r >= 0)											// 右侧正转
    {
        gpio_set(DIR_R, GPIO_HIGH);							// DIR输出高电平
        pwm_duty_updata(PWM_TIM, PWM_R, duty_r);			// 计算占空比
    }
    else													// 右侧反转
    {
        gpio_set(DIR_R, GPIO_LOW);							// DIR输出低电平
        pwm_duty_updata(PWM_TIM, PWM_R, -duty_r);			// 计算占空比
    }
}