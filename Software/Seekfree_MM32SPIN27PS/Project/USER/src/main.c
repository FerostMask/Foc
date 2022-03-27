/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				main
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "headfile.h"
#include "slave.h"
#include "foc.h"
#include "driver.h"

// *************************** 例程说明 ***************************
// 
// 测试需要准备逐飞科技MM32SPIIN27PS核心板一块
// 
// 调试下载需要准备逐飞科技CMSIS-DAP或Jlink调试下载器一个
// 
// 本例程是个空工程 用来给同学们移植使用
// 
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project  clean  等待下方进度条走完
// 
// *************************** 例程说明 ***************************

// **************************** 宏定义 ****************************
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************
char val[2] = {0x3F, 0xFF};
char *valWrite = {0xFF, 0xFF};

// **************************** 变量定义 ****************************

// **************************** 代码区域 ****************************
int main(void)
{
	board_init(true);																// 初始化 debug 输出串口
	systick_delay_ms(10); // 延时0.1秒开机，防止奇奇怪怪的问题
	ips114_init();
	//此处编写用户代码(例如：外设初始化代码等)
	spiDevice.init(); // SPI设备初始化
	driver.init(); // 驱动器初始化
	foc.init(&foc, &driver, &encoder);
	spiDevice.drv->info();
	tim_interrupt_init_ms(TIM_2, 1, 0);
//	tim_interrupt_init_ms(TIM_3, 5, 1);
	//此处编写用户代码(例如：外设初始化代码等)

	while(1)
	{
//		foc.transform(&foc);
//		uart_putstr(UART_2, "Hello!");
//		driver.sensor->sampling(driver.sensor);
		//此处编写需要循环执行的代码
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
		//此处编写需要循环执行的代码
	}
}
// **************************** 代码区域 ****************************
