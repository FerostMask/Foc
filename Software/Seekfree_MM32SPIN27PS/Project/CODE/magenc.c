/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "magenc.h"
#include "zf_systick.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "string.h"
#include "zf_gpio.h"

int blink = 0;
int valGet = 0;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
void encoderInit(void)
{
	gpio_init(B3, GPO, 1, GPO_PUSH_PULL); // SCK
	gpio_init(B5, GPO, 1, GPO_PUSH_PULL); // MOSI
	gpio_init(A15, GPO, 1, GPO_PUSH_PULL); // CS
	gpio_init(B4, GPI, 1, GPI_FLOATING_IN); // MISO
}

void readEncoder(void)
{
	int rsInt = 0;
	int j = 0;
	systick_delay_us(1);
	gpio_set(B3, 0);
	gpio_set(A15, 0);
	blink = 0;
	systick_delay_us(1);
	for(int i = 0; i < 32; i++){
		blink = !blink;
		gpio_set(B3, blink);
//		ips114_showint16(0, 0, blink);
		if(!blink){
			valGet = gpio_get(B4);
			rsInt |= valGet<<(15-j);
			j++;
//			ips114_showint16(100, 3, j);
//			ips114_showint16(0, 1, valGet);
		}
	}
	gpio_set(B3, 0);
	systick_delay_us(1);
	gpio_set(A15, 1);
	ips114_showint16(0, 2, rsInt&0x1fff);
}
