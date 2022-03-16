#include "display.h"





void display_entry(void *parameter)
{
    while(1)
    {
        ips114_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
    }
    
}






void display_init(void)
{
    rt_thread_t tid;
    
    //初始化屏幕
    ips114_init();
    
    //创建显示线程 优先级设置为6 
    tid = rt_thread_create("display", display_entry, RT_NULL, 256, 7, 30);
    
    //启动显示线程
    if(RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
}