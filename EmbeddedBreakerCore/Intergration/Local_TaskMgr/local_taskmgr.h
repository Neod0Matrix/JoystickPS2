#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//主循环函数任务调度管理

//RTOS CPU占用率接口
#ifdef useRTOSinProject
#define RTOSCPUUsage		OSStatTaskCPUUsage				
#else
#define RTOSCPUUsage		0u				
#endif

//优先级任务处理栈总线
extern void prio1TaskBus (void);
extern void prio2TaskBus (void);
extern void prio3TaskBus (void);
extern void prio4TaskBus (void);
extern void Streak_TaskRun (void);							//不使用任务管理器执行			
extern void RTOS_TaskMgr (void);							//调用RTOS

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
