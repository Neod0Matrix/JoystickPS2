#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//STM32低功耗睡眠模式

#define WKUP_KD 		PAin(0)					//PA0 检测是否外部WK_UP按键按下
#define WKUP_EXTI_Line	EXTI_Line0				//总线设置

void Delay (__IO uint32_t nCount);				//延时
void mcu_SleepMode (void);						//睡眠
void mcu_StopMode (void);						//停止
void mcu_SuspendHandler (void);					//挂起
	 
Bool_ClassType Check_WKUP(void);  				//检测WKUP脚的信号
void Sys_Standby (void);						//系统待命
void Sys_Enter_Standby (void);					//系统待机
void WKUP_Init(void); 							//PA0 WKUP唤醒初始化

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
