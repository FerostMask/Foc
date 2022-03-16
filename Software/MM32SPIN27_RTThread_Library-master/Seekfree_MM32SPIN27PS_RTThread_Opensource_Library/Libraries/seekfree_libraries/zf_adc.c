/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				adc
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#include "zf_adc.h"
#include "zf_gpio.h"

ADC_TypeDef *adc_index[2] = {ADC1, ADC2};
uint8 adc_resolution[2] = {ADC_12BIT, ADC_12BIT};

//-------------------------------------------------------------------------------------------------------------------
// @brief		ADC 引脚初始化 内部调用
// @param		ch				选择ADC通道
// @return		void			NULL
// Sample usage:				adc_pin_init(ch);
//-------------------------------------------------------------------------------------------------------------------
static void adc_pin_init (ADCCH_enum ch)
{
	if((ch & 0x0f0) !=  0x020)																	// 确认是引脚通道
	{
		gpio_init((PIN_enum)(ch &0xff), GPI, GPIO_LOW, GPI_ANAOG_IN);
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ADC初始化
// @param		ch				选择ADC通道
// @param		resolution		选择选择通道分辨率(如果同一个模块初始化时设置了不同的分辨率 则第一个初始化的分辨率生效)
// @return		void
// Sample usage:				adc_init(ADC_1, ADC1_CH00_A00, ADC_8BIT);						//初始化A00为ADC功能 分辨率为8位
//-------------------------------------------------------------------------------------------------------------------
void adc_init (ADCN_enum adc, ADCCH_enum ch, ADCRES_enum resolution)
{
	adc_pin_init(ch);																			// 根据引脚编号来初始化引脚

	if((RCC->APB2ENR & (RCC_APB2ENR_ADC1 << adc)) == 0)											// 如果 ADC 未被使能
	{
		RCC->APB2ENR |= (RCC_APB2ENR_ADC1 << adc);												// 使能 ADC 时钟
		RCC->APB2RSTR |= (RCC_APB2RSTR_ADC1 << adc);											// 复位 ADC 外设
		RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADC1 << adc);											// 复位 结束

		adc_resolution[adc] = resolution;														// 记录ADC精度 将在采集时使用 详细请查看 UM <12.5.1 可编程分辨率> 章节
		adc_index[adc]->ADCFG |= ((uint32)resolution << ADC_CFGR_RSLTCTL_Pos);					// ADC 精度修改
		adc_index[adc]->ADCFG |= ADC_CFGR_PRE_10;												// ADC 时钟 10 分频
		adc_index[adc]->ADCR &= ~(ADC_CR_MODE | ADC_CR_ALIGN);									// 清空设置 默认单次转换 数据左对齐

		adc_index[adc]->ADCFG |= ADC_CFGR_ADEN;													// ADC 使能
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ADC转换一次
// @param		ch				选择ADC通道
// @return		uint16			转换的ADC值
// Sample usage:				adc_convert(ADC_1, ADC1_CH00_A00);
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_convert (ADCN_enum adc, ADCCH_enum ch)
{
	adc_index[adc]->ADCHS = (0x00000001 << ((ch&0xf00)>>8));									// 使能对应通道
	adc_index[adc]->ADCR |= ADC_CR_ADST;														// 开始数据转换
	while(((adc_index[adc]->ADSTA ) & ADC_SR_ADIF) == 0);										// 等待数据转换完成
	adc_index[adc]->ADSTA |= ADC_SR_ADIF;														// 清除转换完成标志
	return ((adc_index[adc]->ADDATA & 0xfff) >> adc_resolution[adc]);							// 读取数据 为什么这里要位移请查看 UM <12.5.1 可编程分辨率> 章节
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ADC均值滤波
// @param		ch				选择ADC通道
// @param		count			均值滤波次数
// @return		uint16			转换的ADC值
// Sample usage:				adc_convert(ADC_1, ADC1_CH00_A00, 5);//采集5次 然后返回平均值
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_mean_filter (ADCN_enum adc, ADCCH_enum ch, uint8 count)
{
	uint8 i;
	uint32 sum;

	sum = 0;
	for(i=0; i<count; i++)
	{
		sum += adc_convert(adc, ch);
	}

	return sum/count;
}
