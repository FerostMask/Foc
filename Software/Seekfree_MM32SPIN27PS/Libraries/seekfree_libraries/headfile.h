/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				headfile
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2019-04-30
********************************************************************************************************************/
 
#ifndef _headfile_h
#define _headfile_h


#include <stdint.h>

#include "board.h"
#include "SEEKFREE_PRINTF.h"


//------逐飞科技单片机外设驱动头文件
#include "zf_adc.h"
#include "zf_camera.h"
#include "zf_exti.h"
#include "zf_flash.h"
#include "zf_gpio.h"
#include "zf_spi.h"
#include "zf_systick.h"
#include "zf_pit.h"
#include "zf_pwm.h"
#include "zf_tim.h"
#include "zf_uart.h"

//------逐飞科技产品驱动头文件
#include "SEEKFREE_18TFT.h"
#include "SEEKFREE_7725.h"
#include "SEEKFREE_ABSOLUTE_ENCODER.h"
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_ICM20602.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "SEEKFREE_L3G4200D.h"
#include "SEEKFREE_MMA8451.h"
#include "SEEKFREE_MPU6050.h"
#include "SEEKFREE_OLED.h"
#include "SEEKFREE_TSL1401.h"
#include "SEEKFREE_UART_7725.h"
#include "SEEKFREE_VIRSCO.h"
#include "SEEKFREE_WIRELESS.h"
#include "SEEKFREE_MT9V03X.h"

#endif

