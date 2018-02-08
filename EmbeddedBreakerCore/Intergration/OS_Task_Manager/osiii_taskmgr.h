#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	uC/OS-III本地配置
	堆栈设置：UCOSII在64-128字节左右，UCOSIII在128-256字节左右
	太大容易造成SRAM溢出，设置太小会触发硬件错误
*/

#define SmallTaskStackSize				(CPU_STK_SIZE)64u	//小空间任务栈
#define OrdTaskStackSize				(CPU_STK_SIZE)128u	//常用堆栈大小
#define MainTaskStackSize				(CPU_STK_SIZE)256u	//主任务堆栈大小

//设置任务执行优先级
typedef enum 
{
	DivTaskSPrio = (OS_PRIO)3,								//起始任务，优先级最高(3)
	DivTask1Prio = (OS_PRIO)4,
	DivTask2Prio = (OS_PRIO)6,
	DivTask3Prio = (OS_PRIO)7,
	DivTask4Prio = (OS_PRIO)11,								//任务4内存处理，放到最低优先级
} Task_ExePrio_Setting;

//设置任务堆栈大小(按占用资源分配)
typedef enum 
{
	DivTaskSStkSize = OrdTaskStackSize,						//任务起始栈
	DivTask1StkSize = OrdTaskStackSize,
	DivTask2StkSize = OrdTaskStackSize,
	DivTask3StkSize = MainTaskStackSize,
	DivTask4StkSize = SmallTaskStackSize,
} Task_StackSize_Setting;

//任务放置划分函数(OS调用)
void divTaskNbrS (void *p_arg);								//任务起始函数				
void divTaskNbr1 (void *p_arg);							
void divTaskNbr2 (void *p_arg);	
void divTaskNbr3 (void *p_arg);					
void divTaskNbr4 (void *p_arg);			

#define TaskStack_Nbr					5u					//存储区中存储块数量，对应任务数量(包括内存管理和起始任务)

void ucosiii_TaskMgr (void);								//ucosiii工作预处理
void ShareResources_Handler (u8 *resource, const void *task_str, u16 delay_time);//资源共享访问
void Semaphore_Handler (void);								//信号量处理
void MemStack_ReqHandler (void);							//内存块申请处理
void osiiiTaskFuction (		void (*ActualExeFunction)(void), 
							u8* srStr, 
							u8 srpTime, 
							u16 dlyTime);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
