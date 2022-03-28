/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "rtthread.h"
#include "display.h"
#include "SEEKFREE_IPS114_SPI.h"
/*--------------------------------------------------------------*/
/* 						   枚举类型定义							*/
/*==============================================================*/

enum THREAD_INFO
{
    THREAD_PRIORITY = 7,
    THREAD_STACK_SIZE = 256,
    THREAD_TIMESLICE = 30,
};

/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/

/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
static rt_thread_t tidDisplay = RT_NULL;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
void displayEntry(void)
{
    static char count = 0;
    // while (1)
    // {
    //     ips114_showint8(0, 0, count);
    //     count++;
    //     rt_thread_mdelay(100);
    // }
}

void displayInit(void)
{
    ips114_init();
    ips114_showstr(0, 0, "Welcome back!");
    tidDisplay = rt_thread_create("display", displayEntry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tidDisplay != RT_NULL)
    {
        rt_thread_startup(tidDisplay);
        ips114_showstr(0, 0, "thread start up");
    }

    return 0;
};
