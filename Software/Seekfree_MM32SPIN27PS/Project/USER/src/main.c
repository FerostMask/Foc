/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				main
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "headfile.h"
#include "slave.h"
#include "foc.h"
#include "driver.h"

// *************************** ����˵�� ***************************
// 
// ������Ҫ׼����ɿƼ�MM32SPIIN27PS���İ�һ��
// 
// ����������Ҫ׼����ɿƼ�CMSIS-DAP��Jlink����������һ��
// 
// �������Ǹ��չ��� ������ͬѧ����ֲʹ��
// 
// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project  clean  �ȴ��·�����������
// 
// *************************** ����˵�� ***************************

// **************************** �궨�� ****************************
// **************************** �궨�� ****************************

// **************************** �������� ****************************
char val[2] = {0x3F, 0xFF};
char *valWrite = {0xFF, 0xFF};

// **************************** �������� ****************************

// **************************** �������� ****************************
int main(void)
{
	board_init(true);																// ��ʼ�� debug �������
	systick_delay_ms(10); // ��ʱ0.1�뿪������ֹ����ֵֹ�����
	ips114_init();
	//�˴���д�û�����(���磺�����ʼ�������)
	spiDevice.init(); // SPI�豸��ʼ��
	driver.init(); // ��������ʼ��
	foc.init(&foc, &driver, &encoder);
	spiDevice.drv->info();
	tim_interrupt_init_ms(TIM_2, 1, 0);
//	tim_interrupt_init_ms(TIM_3, 5, 1);
	//�˴���д�û�����(���磺�����ʼ�������)

	while(1)
	{
//		foc.transform(&foc);
//		uart_putstr(UART_2, "Hello!");
//		driver.sensor->sampling(driver.sensor);
		//�˴���д��Ҫѭ��ִ�еĴ���
//		spiDevice.enc->read();
//		driver.motor->cycleRotate(&driver, 1, 1500, CLOCKWISE);
//		float temp = encoder.absAngle;
//		while(temp == encoder.absAngle){
//			encoder.read();
//		}
//		
//		ips114_showfloat(0, 0, encoder.absAngle, 3, 3);
//		systick_delay_ms(100);
//		driver.motor->cycleRotate(&driver, 7, 1500, ANTICLOCKWISE);
//		systick_delay_ms(100);
		//�˴���д��Ҫѭ��ִ�еĴ���
	}
}
// **************************** �������� ****************************
