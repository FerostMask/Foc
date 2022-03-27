/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file				isr
 * @company			成都逐飞科技有限公司
 * @author			逐飞科技(QQ3184284598)
 * @version			查看doc内version文件 版本说明
 * @Software			IAR 8.3 or MDK 5.28
 * @Target core		MM32SPIN2XPs
 * @Taobao			https://seekfree.taobao.com/
 * @date				2020-11-23
 ********************************************************************************************************************/

#include "headfile.h"
#include "isr.h"

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM1->SR; // 读取中断状态
	TIM1->SR &= ~state;		 // 清空中断状态
	rt_interrupt_leave();
}

void TIM2_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM2->SR; // 读取中断状态
	TIM2->SR &= ~state;		 // 清空中断状态
	rt_interrupt_leave();
}

void TIM3_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM3->SR; // 读取中断状态
	TIM3->SR &= ~state;		 // 清空中断状态
	rt_interrupt_leave();
}

void TIM8_BRK_UP_TRG_COM_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM8->SR; // 读取中断状态
	TIM8->SR &= ~state;		 // 清空中断状态
	rt_interrupt_leave();
}

void TIM14_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM14->SR; // 读取中断状态
	TIM14->SR &= ~state;	  // 清空中断状态
	rt_interrupt_leave();
}

void TIM16_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM16->SR; // 读取中断状态
	TIM16->SR &= ~state;	  // 清空中断状态
	rt_interrupt_leave();
}

void TIM17_IRQHandler(void)
{
	rt_interrupt_enter();
	uint32 state = TIM17->SR; // 读取中断状态
	TIM17->SR &= ~state;	  // 清空中断状态
	rt_interrupt_leave();
}

void UART2_IRQHandler(void)
{
	rt_interrupt_enter();
	if (UART2->ISR & UART_ISR_TX_INTF) // 串口发送缓冲空中断
	{
		UART2->ICR |= UART_ICR_TXICLR; // 清除中断标志位
	}
	if (UART2->ISR & UART_ISR_RX_INTF) // 串口接收缓冲中断
	{
		switch (camera_type) // 查询摄像头类型 未初始化摄像头则此处会进入default
		{
		case CAMERA_BIN_UART:			 // 串口小钻风
			ov7725_cof_uart_interrupt(); // 调用串口小钻风的串口接收处理
			break;
		case CAMERA_GRAYSCALE:		 // 总钻风
			mt9v03x_uart_callback(); // 调用总钻风的串口接收处理
			break;
		default:
			break;
		}
		UART2->ICR |= UART_ICR_RXICLR; // 清除中断标志位
	}
	rt_interrupt_leave();
}

void EXTI0_1_IRQHandler(void)
{
	rt_interrupt_enter();
	// 检测与清除中断标志可以根据实际应用进行删改
	if (EXTI_GetITStatus(EXTI_Line0)) // 检测 line0 是否触发
	{
		EXTI_ClearFlag(EXTI_Line0); // 清除 line0 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line1)) // 检测 line1 是否触发
	{
		EXTI_ClearFlag(EXTI_Line1); // 清除 line1 触发标志
	}
	//	EXTI_ClearFlag(0x0003);															// EXTI_Line0 | EXTI_Line1

	rt_interrupt_leave();
}

void EXTI2_3_IRQHandler(void)
{
	rt_interrupt_enter();
	// 检测与清除中断标志可以根据实际应用进行删改
	if (EXTI_GetITStatus(EXTI_Line2)) // 检测 line2 是否触发
	{
		EXTI_ClearFlag(EXTI_Line2); // 清除 line2 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line3)) // 检测 line3 是否触发
	{
		EXTI_ClearFlag(EXTI_Line3); // 清除 line3 触发标志
	}
	//	EXTI_ClearFlag(0x000C);															// EXTI_Line2 | EXTI_Line3

	rt_interrupt_leave();
}

void EXTI4_15_IRQHandler(void)
{
	rt_interrupt_enter();
	// 检测与清除中断标志可以根据实际应用进行删改
	if (EXTI_GetITStatus(EXTI_Line4)) // 检测 line4 是否触发
	{
		EXTI_ClearFlag(EXTI_Line4); // 清除 line4 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line5)) // 检测 line5 是否触发
	{
		EXTI_ClearFlag(EXTI_Line5); // 清除 line5 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line6)) // 检测 line6 是否触发
	{
		EXTI_ClearFlag(EXTI_Line6); // 清除 line6 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line7)) // 检测 line7 是否触发
	{
		switch (camera_type) // 查询摄像头类型 未初始化摄像头则此处会进入default
		{
		case CAMERA_BIN_UART: // 串口小钻风
			ov7725_uart_vsync();
			break;
		case CAMERA_GRAYSCALE: // 总钻风
			mt9v03x_vsync();
			break;
		default:
			break;
		}
		EXTI_ClearFlag(EXTI_Line7); // 清除 line8 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line8)) // 检测 line8 是否触发
	{
		EXTI_ClearFlag(EXTI_Line8); // 清除 line8 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line9)) // 检测 line9 是否触发
	{
		EXTI_ClearFlag(EXTI_Line9); // 清除 line9 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line10)) // 检测 line10 是否触发
	{
		EXTI_ClearFlag(EXTI_Line10); // 清除 line10 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line11)) // 检测 line11 是否触发
	{
		EXTI_ClearFlag(EXTI_Line11); // 清除 line11 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line12)) // 检测 line12 是否触发
	{
		EXTI_ClearFlag(EXTI_Line12); // 清除 line12 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line13)) // 检测 line13 是否触发
	{
		EXTI_ClearFlag(EXTI_Line13); // 清除 line13 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line14)) // 检测 line14 是否触发
	{
		EXTI_ClearFlag(EXTI_Line14); // 清除 line14 触发标志
	}
	if (EXTI_GetITStatus(EXTI_Line15)) // 检测 line15 是否触发
	{
		EXTI_ClearFlag(EXTI_Line15); // 清除 line15 触发标志
	}
	//	EXTI_ClearFlag(0xFFF0);															// EXTI_Line4 | ... | EXTI_Line15

	rt_interrupt_leave();
}

void DMA1_Channel1_IRQHandler(void)
{
	rt_interrupt_enter();
	if (SET == DMA_GetFlagStatus(DMA1_FLAG_TC1)) // 判断触发通道
	{
		DMA_ClearFlag(DMA1_FLAG_TC1); // 清空该通道中断标志
	}
	rt_interrupt_leave();
}

void DMA1_Channel2_3_IRQHandler(void)
{
	rt_interrupt_enter();
	if (SET == DMA_GetFlagStatus(DMA1_FLAG_TC2)) // 判断触发通道
	{
		DMA_ClearFlag(DMA1_FLAG_TC2); // 清空该通道中断标志
	}
	if (SET == DMA_GetFlagStatus(DMA1_FLAG_TC3)) // 判断触发通道
	{
		DMA_ClearFlag(DMA1_FLAG_TC3); // 清空该通道中断标志
	}
	rt_interrupt_leave();
}

void DMA1_Channel4_5_IRQHandler(void)
{
	rt_interrupt_enter();
	if (SET == DMA_GetFlagStatus(DMA1_FLAG_TC4)) // 判断触发通道
	{
		DMA_ClearFlag(DMA1_FLAG_TC4); // 清空该通道中断标志
		switch (camera_type)		  // 查询摄像头类型 未初始化摄像头则此处会进入default
		{
		case CAMERA_BIN_UART: // 串口小钻风
			ov7725_uart_dma();
			break;
		case CAMERA_GRAYSCALE: // 总钻风
			mt9v03x_dma();
			break;
		default:
			break;
		}
	}
	if (SET == DMA_GetFlagStatus(DMA1_FLAG_TC5)) // 判断触发通道
	{
		DMA_ClearFlag(DMA1_FLAG_TC5); // 清空该通道中断标志
	}
	rt_interrupt_leave();
}

#ifdef Will_never_be_defined
WWDG_IRQHandler
	PVD_IRQHandler
		PWM_IRQHandler
			FLASH_IRQHandler
				RCC_IRQHandler
					EXTI0_1_IRQHandler
						EXTI2_3_IRQHandler
							EXTI4_15_IRQHandler
								HWDIV_IRQHandler
									DMA1_Channel1_IRQHandler
										DMA1_Channel2_3_IRQHandler
											DMA1_Channel4_5_IRQHandler
												ADC1_IRQHandler
													TIM1_BRK_UP_TRG_COM_IRQHandler
														TIM1_CC_IRQHandler
															TIM2_IRQHandler
																TIM3_IRQHandler
																	TIM8_BRK_UP_TRG_COM_IRQHandler
																		TIM8_CC_IRQHandler
																			TIM14_IRQHandler
																				ADC2_IRQHandler
																					TIM16_IRQHandler
																						TIM17_IRQHandler
																							I2C1_IRQHandler
																								COMP1_2_3_4_5_IRQHandler
																									SPI1_IRQHandler
																										SPI2_IRQHandler
																											UART1_IRQHandler
																												UART2_IRQHandler
#endif
