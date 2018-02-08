#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//欠压保护监测
//初始化PC10作为监测IO口，低电平触发

//#define LVD_IO PCin(10)

#define LVD_IO  	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)	//读取IO口状态

void LVD_IO_Init (void);										//IO口初始化
void LVD_EW_Handler (void);										//报警处理

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
