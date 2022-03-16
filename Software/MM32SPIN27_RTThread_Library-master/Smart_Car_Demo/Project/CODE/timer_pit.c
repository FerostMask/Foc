#include "encoder.h"
#include "motor.h"
#include "timer_pit.h"
#include "elec.h"

void timer1_pit_entry(void *parameter)
{
    static uint32 time;
    time++;
    
    //�ɼ�����������
    //get_icm20602_gyro_spi();
    
    if(0 == (time%100))
    {
        //�ɼ�����������
        encoder_get();
    }

    if(0 == (time%5))
    {
        //�ɼ����ٶ�����
        //get_icm20602_accdata_spi();

        //�ɼ�����ź�
        elec_get();
        
        //���ݵ���źż��㳵��λ��
        elec_calculate();
    }
    
    //���Ƶ��ת��
    motor_control(10000, 10000);
    
}


void timer_pit_init(void)
{
    rt_timer_t timer;
    
    //����һ����ʱ�� ��������
    timer = rt_timer_create("timer1", timer1_pit_entry, RT_NULL, 1, RT_TIMER_FLAG_PERIODIC);
    
    //������ʱ��
    if(RT_NULL != timer)
    {
        rt_timer_start(timer);
    }

    
}