#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//系统状态请求

extern uint32_t devID[3];						//芯片设备ID号存储数组

extern void Get_MCU_SerialNumID (void);			//获取MCU ID
extern uint32_t Get_BSP_CPU_ClkFreq (void);		//获取CPU主频
extern void PreTesFuncAlgInterface (void);		//算法功能测试
extern void displaySystemInfo (void);			//协议请求

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
