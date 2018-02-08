#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//GPIO初始化库

//是否对板外输出(需要ULN2003A反相器)
typedef enum {EBO_Enable = 1, EBO_Disable = !EBO_Enable} ExternBoardOutput;
//起始高低状态
typedef enum {IHL = 0, ILL = 1, NI = 2} Init_LevelStatus;

#define GPIORemapSettingNULL	(uint32_t)0x00000000				//IO无重映射，无效参数
#define GPIO_Input_Speed		(GPIOSpeed_TypeDef)0u				//输入无速度，无效参数

//GPIO通用初始化配置
void ucGPIO_Config_Init (	uint32_t 			rcc_axbx_gpiox,		//RCC时钟总线
							GPIOMode_TypeDef 	mode,				//IO模式
							GPIOSpeed_TypeDef 	speed,				//IO速度
							uint32_t			remap,				//重映射
							uint16_t 			pin,				//PIN脚
							GPIO_TypeDef* 		gpiox,				//GPIO总线
							Init_LevelStatus	ils_flag,			//初始化高低电平
							ExternBoardOutput	ebo_flag);			//是否外接输出到板子上

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
