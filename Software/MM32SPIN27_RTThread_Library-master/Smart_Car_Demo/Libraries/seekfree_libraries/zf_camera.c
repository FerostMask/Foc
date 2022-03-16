/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				camera
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_camera.h"
#include "zf_gpio.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief		摄像头DMA初始化
// @param		dma_ch				DAM通道
// @param		src_addr			源地址
// @param		des_addr			目标地址
// @param		size				数据长度
// @return		void
// Sample usage:					camera_dma_init(DMA1_Channel4, GPIOA->ODR, GPIOC->ODR, 8);
//-------------------------------------------------------------------------------------------------------------------
void camera_dma_init (DMA_Channel_TypeDef* dma_ch, uint32 src_addr, uint32 des_addr, uint32 size)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);
	DMA_DeInit(dma_ch);

	//MDA配置初始化
	DMA_InitStructure.DMA_PeripheralBaseAddr = src_addr;										// 源地址
	DMA_InitStructure.DMA_MemoryBaseAddr = des_addr;											// 目标地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;											// 外设作为源
	DMA_InitStructure.DMA_BufferSize = size;													// 传输多少个数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;							// 外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;										// 内存地址依次+1
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;						// 外设每次传输一个字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;								// 内存每次传输一个字节
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;												// 非循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;										// 优先级最高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;												// 非内存到内存模式
	DMA_InitStructure.DMA_Auto_reload = DMA_Auto_Reload_Enable;
	DMA_Init(dma_ch, &DMA_InitStructure);

	DMA_ITConfig(dma_ch, DMA_CCR_TCIE, ENABLE);													// 配置DMA传输完成中断
	DMA_Cmd(dma_ch, ENABLE);																	// 开启DMA1
	DMA_Cmd(dma_ch, DISABLE);																	// 开启DMA1
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		摄像头GPIO触发定时器初始化
// @param		timern				定时器通道
// @param		pin					引脚号
// @return		void
// Sample usage:					camera_tim_etr_init(TIM_3_ETR_D06);  						// 定时器ERT触发是固定的引脚禁止随意修改
//-------------------------------------------------------------------------------------------------------------------
void camera_tim_etr_init (TIM_ETR_PIN_enum pin)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	switch((pin&0xF000))
	{
		case 0x0000:	RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);break;
		case 0x2000:	RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2, ENABLE);break;
		case 0x3000:	RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);break;
	}

	afio_init((PIN_enum)(pin &0xff), GPI, (GPIOAF_enum)((pin &0xf00)>>8), GPI_PULL_UP);			// 提取对应IO索引 AF功能编码

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(tim_index[((pin&0xF000)>>12)], &TIM_TimeBaseStructure);

	TIM_ETRClockMode1Config(tim_index[((pin&0xF000)>>12)], TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_Inverted, 0x00);
	TIM_SelectInputTrigger(tim_index[((pin&0xF000)>>12)], TIM_TS_ETRF);
	TIM_SelectSlaveMode(tim_index[((pin&0xF000)>>12)], TIM_SlaveMode_Reset);					//TIM从模式：触发信号的上升沿重新初始化计数器和触发寄存器的更新事件
	TIM_SelectMasterSlaveMode(tim_index[((pin&0xF000)>>12)], TIM_MasterSlaveMode_Enable);		//启动定时器的被动触发

	TIM_Cmd(tim_index[((pin&0xF000)>>12)], ENABLE);
	TIM_DMACmd(tim_index[((pin&0xF000)>>12)], TIM_DMA_Trigger, ENABLE);							//使能TIM_DMA
}
