/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,��ɿƼ�
* All rights reserved.
* ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file				board
* @company			�ɶ���ɿƼ����޹�˾
* @author			��ɿƼ�(QQ3184284598)
* @version			�鿴doc��version�ļ� �汾˵��
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "board.h"
#include "zf_uart.h"
#include <rtthread.h>
#include "mm32_reg_redefine_v1.h"

extern uint32_t SystemCoreClock;

//finsh������մ������ݣ���ͨ���ڴ����ж��ڷ����ʼ���finsh�߳̽����ʼ����л�ȡ��
rt_mailbox_t uart_mb;

static uint32_t systick_config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }

    SysTick->LOAD = ticks - 1; 
    nvic_init(SysTick_IRQn, 3, ENABLE);
    SysTick->VAL  = 0;
    SysTick->CTRL = 0x07;  
    
    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 700

static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif


void rt_hw_board_init()
{
    systick_config(SystemCoreClock / RT_TICK_PER_SECOND);
    
    board_init(1);
    
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
    
    uart_mb = rt_mb_create("uart_mb", 10, RT_IPC_FLAG_FIFO);
}


void SysTick_Handler(void)
{
    rt_interrupt_enter();

    rt_tick_increase();


    rt_interrupt_leave();
}

void rt_hw_console_output(const char *str)
{
    while(RT_NULL != *str)
    {
        if('\n' == *str)
        {
            uart_putchar(DEBUG_UART, '\r');
        }
        uart_putchar(DEBUG_UART, *str++);
    }
}



char rt_hw_console_getchar(void)
{
    uint32 dat;
    //�ȴ��ʼ�
    rt_mb_recv(uart_mb, &dat, RT_WAITING_FOREVER);
    //uart_getchar(DEBUG_UART, &dat);
    return (char)dat;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief		���İ��ʼ��
// @param		debug_enable	�Ƿ���Ĭ�� debug ��� Ĭ�� UART1 
// @return		void
// Sample usage:				board_init(TRUE);
//-------------------------------------------------------------------------------------------------------------------
void board_init (bool debug_enable)
{
	if(debug_enable)
    {
        uart_init(DEBUG_UART, DEBUG_UART_BAUD, DEBUG_UART_TX, DEBUG_UART_RX);					// Ĭ�ϳ�ʼ�� UART1 ����֧�� printf ���
    }

    uart_rx_irq(DEBUG_UART, 1);
}

void UART1_IRQHandler(void)
{
    uint8 dat;
    rt_interrupt_enter();
	if(UART1->ISR & UART_ISR_TX_INTF)		// ���ڷ��ͻ�����ж�
	{
		UART1->ICR |= UART_ICR_TXICLR;		// ����жϱ�־λ
	}
	if(UART1->ISR & UART_ISR_RX_INTF)		// ���ڽ��ջ����ж�
	{
		uart_getchar(DEBUG_UART, &dat);
        rt_mb_send(uart_mb, dat);           // �����ʼ�
        UART1->ICR |= UART_ICR_RXICLR;		// ����жϱ�־λ
	}
    rt_interrupt_leave();
}

