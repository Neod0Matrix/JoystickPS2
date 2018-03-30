#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	本地任务调度管理
	在任务数量不多的情况下，裸跑比RTOS速度快，
	在任务数量多占用时长差距大时RTOS性能更好
*/

void prio1TaskBus (void)
{
	OrderResponse_Handler();						//指令响应函数
}

void prio2TaskBus (void)
{
	LVD_EW_Handler();								//输入电压低压监测
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	Modules_NonInterruptTask();
#ifdef useRTOSinProject
	Semaphore_Handler();							//信号量处理
#endif
}

void prio3TaskBus (void)
{
#ifdef useRTOSinProject
	CPU_SR_ALLOC();
#endif
	MCU_Temperature_Detector();						//温度监测
#ifdef useRTOSinProject
	OS_CRITICAL_ENTER();
#endif
	UIScreen_DisplayHandler();						//UI显示器
#ifdef useRTOSinProject
	OS_CRITICAL_EXIT();
#endif
}

void prio4TaskBus (void)
{
	Modules_RTC_TaskScheduler();					//RTC任务计划
#ifdef useRTOSinProject
	MemStack_ReqHandler();							//内存申请处理
#endif
}

//不使用任务管理器的顺序执行
void Streak_TaskRun (void)
{
	//循环任务
	while (True) 
	{
		prio1TaskBus();
		prio2TaskBus();
		prio3TaskBus();
		prio4TaskBus();
	}
}

//RTOS调用接口
void RTOS_TaskMgr (void)
{
	ucosiii_TaskMgr();										//ucos
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
