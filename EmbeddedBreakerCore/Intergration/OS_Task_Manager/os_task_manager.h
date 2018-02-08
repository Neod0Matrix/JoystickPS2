#ifndef __OS_TASK_MANAGER_H__
#define __OS_TASK_MANAGER_H__
#include "SYS.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//UCOS使用的任务管理器本地管理

#define Ordinary_Task_Stack_Size		64		//常用堆栈大小
#define Main_Task_Stack_Size			128		//主任务堆栈大小

//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 		//开始任务的优先级设置为最低
#define START_STK_SIZE  				Ordinary_Task_Stack_Size//设置任务堆栈大小
void start_task (void *pdata);					//任务函数

//LED1信号量
#define LED1_Signal_Prio				8
#define LED1_Signal_Stk_Size			Ordinary_Task_Stack_Size
void LED1_Signal_Task (void *pdata);

//GPS数据
#define GPS_App_Prio					7
#define GPS_App_Stk_Size				Ordinary_Task_Stack_Size
void GPS_App_Task (void *pdata);

//键盘响应
#define KeyBoardAck_Prio				6
#define KeyBoardAck_Stk_Size			Ordinary_Task_Stack_Size
void KeyBoardAck_Task (void *pdata);

//系统状态监测函数
#define System_Detect_Prio				5
#define System_Detect_Stk_Size			Ordinary_Task_Stack_Size
void System_Detect_Task (void *pdata);

//响应外部指令调试函数
#define Order_Responder_Prio			3
#define Order_Responder_Stk_Size		Main_Task_Stack_Size
void Order_Responder_Task (void *pdata);

void uCOS_Task_Manager (void);					//UCOS工作预处理
void Semaphore_Handler (void);					//信号量处理

#endif

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
