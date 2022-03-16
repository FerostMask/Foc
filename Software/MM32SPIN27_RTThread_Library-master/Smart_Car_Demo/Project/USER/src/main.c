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
#include "display.h"
#include "timer_pit.h"
#include "encoder.h"
#include "buzzer.h"
#include "button.h"
#include "motor.h"
#include "elec.h"
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

// **************************** 变量定义 ****************************

// **************************** 代码区域 ****************************




rt_sem_t camera_sem;


int main(void)
{
	//此处编写用户代码(例如：外设初始化代码等)

    camera_sem = rt_sem_create("camera", 0, RT_IPC_FLAG_FIFO);
    
    
    mt9v03x_init();
    //icm20602_init_spi();
    
    display_init();
    encoder_init();
    buzzer_init();
    button_init();
    motor_init();
    elec_init();
    
    timer_pit_init();
    while(1)
    {
        //等待摄像头采集完毕
        rt_sem_take(camera_sem, RT_WAITING_FOREVER);
        //rt_thread_mdelay(10);
        //开始处理摄像头图像
        
    }

}
// **************************** 代码区域 ****************************
