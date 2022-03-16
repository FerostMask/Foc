#include "encoder.h"

#define ENCODER1_TIM		TIM_1
#define ENCODER1_A			TIM_1_ENC1_A08
#define ENCODER1_B			TIM_1_ENC2_A01

#define ENCODER2_TIM		TIM_8
#define ENCODER2_A			TIM_8_ENC1_C00
#define ENCODER2_B			TIM_8_ENC2_C01

uint16 speed_l, speed_r;

void encoder_init(void)
{
    tim_encoder_init(ENCODER1_TIM, ENCODER1_A, ENCODER1_B);
	tim_encoder_init(ENCODER2_TIM, ENCODER2_A, ENCODER2_B);
}


void encoder_get(void)
{
    speed_l = tim_encoder_get_count(ENCODER1_TIM);
    tim_encoder_rst(ENCODER1_TIM);
    speed_r = tim_encoder_get_count(ENCODER2_TIM);
    tim_encoder_rst(ENCODER2_TIM);
}