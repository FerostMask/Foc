/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				flash
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core		MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#ifndef _zf_flash_h
#define _zf_flash_h

#include "common.h"
#include "hal_flash.h"

#define FLASH_BASE_ADDR			(0x08000000)
#define FLASH_PAGE_SIZE			(0x00000400)				// 1K byte
#define FLASH_SECTION_SIZE		(FLASH_PAGE_SIZE*4)			// 4K byte

// 枚举 Flash 扇区	此枚举定义不允许用户修改
typedef enum
{
	FLASH_SECTION_00,
	FLASH_SECTION_01,
	FLASH_SECTION_02,
	FLASH_SECTION_03,
	FLASH_SECTION_04,
	FLASH_SECTION_05,
	FLASH_SECTION_06,
	FLASH_SECTION_07,
	FLASH_SECTION_08,
	FLASH_SECTION_09,
	FLASH_SECTION_10,
	FLASH_SECTION_11,
	FLASH_SECTION_12,
	FLASH_SECTION_13,
	FLASH_SECTION_14,
	FLASH_SECTION_15,
	FLASH_SECTION_16,
	FLASH_SECTION_17,
	FLASH_SECTION_18,
	FLASH_SECTION_19,
	FLASH_SECTION_20,
	FLASH_SECTION_21,
	FLASH_SECTION_22,
	FLASH_SECTION_23,
	FLASH_SECTION_24,
	FLASH_SECTION_25,
	FLASH_SECTION_26,
	FLASH_SECTION_27,
	FLASH_SECTION_28,
	FLASH_SECTION_29,
	FLASH_SECTION_30,
	FLASH_SECTION_31,
}FLASH_SECTION_enum;

// 枚举 Flash 页索引	此枚举定义不允许用户修改
typedef enum
{
	FLASH_PAGE_0,
	FLASH_PAGE_1,
	FLASH_PAGE_2,
	FLASH_PAGE_3,
}FLASH_PAGE_enum;

uint8		flash_check				(FLASH_SECTION_enum sector_num, FLASH_PAGE_enum page_num);
uint8		flash_erase_page		(FLASH_SECTION_enum sector_num, FLASH_PAGE_enum page_num);
void		flash_page_read			(FLASH_SECTION_enum sector_num, FLASH_PAGE_enum page_num, uint32 *buf, uint16 len);
uint8		flash_page_program		(FLASH_SECTION_enum sector_num, FLASH_PAGE_enum page_num, const uint32 *buf, uint16 len);

#endif
