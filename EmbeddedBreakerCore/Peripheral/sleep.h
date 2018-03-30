#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//STM32低功耗睡眠模式

#define WKUP_KD 		PAin(0)					//PA0 检测是否外部WK_UP按键按下
#define WKUP_EXTI_Line	EXTI_Line0				//总线设置

//全局睡眠标志
typedef enum {SysIntoSleep = 1, SysOrdWork = !SysIntoSleep} Is_SystemSleep;
extern Is_SystemSleep globalSleepflag;

//软件复位/软件睡眠
typedef enum {sys_reset = 0, sys_sleep = !sys_reset} SYS_ResetorSleep;

void Delay (__IO uint32_t nCount);				//延时
Bool_ClassType Check_WKUP (void);
void Sys_Standby (void);						//系统待命
void Sys_Enter_Standby (void);					//系统待机
void WKUP_Init(void); 							//PA0 WKUP唤醒初始化
void Protocol_CtrlResetorSuspend (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
