#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	模块对框架EmbeddBreakerCore的链接
	该文件写入对框架的函数调用支持
*/

#include "ps2.h"											//手柄底层硬件驱动

//模块声明
#define _Modules_Type_			"Joystick"					//模块类型
#define _Modules_Name_			"JoystickPS2"				//模块名称
#define _Modules_Version_ 		"v0p2_LTE"					//长期演进版

//模块使用的协议链接，尽量整合到一条
#define MDLS					0x1B
#define Modules_Protocol 		{DH, MDLS, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DMAX, DT}

//是否开启PS2打印测试
typedef enum {PS2P_Enable = 1, PS2P_Disable = !PS2P_Enable} PS2_PrintKeyValue;
extern PS2_PrintKeyValue 			PS2P_Switch;

//urc开源链接编号
typedef enum
{
	urc_ps2p	= 17,
} Modules_SwitchNbr;

//裁去config.h中的定义放到这里来重新定义urc协议长度
#define Module_Add_urcOption_Count	1u
#define Max_Option_Value		(Module_Add_urcOption_Count + FrameDefault_urcOption_Count)			
//裁去ui.h中定义的总切屏数到这里来重新定义
#define Module_Add_oledScreen_Count	1u
#define ScreenPageCount			(Module_Add_oledScreen_Count + FrameDefault_oledScreen_Count)	

//对外API接口
void Modules_UniResConfig (void);							//选项设置，链接到Universal_Resource_Config函数的模块库
void Modules_URCMap (void);									//模块选项映射表，链接到urcMapTable_Print函数
void Modules_urcDebugHandler (u8 ed_status, Modules_SwitchNbr sw_type);//选项处理，链接到pclURC_DebugHandler函数
void Modules_ProtocolTask (void);							//协议调用指令响应，链接到OrderResponse_Handler函数
void OLED_ScreenModules_Const (void);						//OLED常量显示屏，链接到OLED_DisplayInitConst和UIScreen_DisplayHandler函数
void OLED_DisplayModules (u8 page);							//OLED模块调用数据显示，链接到UIScreen_DisplayHandler函数
void Modules_HardwareInit (void);							//硬件底层初始化任务，链接到bspPeriSysCalls函数
void Modules_ExternInterruptInit (void);					//硬件底层外部中断初始化，链接到EXTI_Config_Init函数
void Modules_NonInterruptTask (void);						//模块非中断任务，链接到local_taskmgr.c，默认添加到第二任务
void Modules_InterruptTask (void);							//模块中断任务，链接到time_base.c TIM2_IRQHandler函数中
void Modules_RTC_TaskScheduler (void);						//基于RTC时间的任务计划，链接到local_taskmgr.c，默认添加到第四任务
void Modules_StatusReqHandler (void);						//模块状态内容打印请求，链接到sta_req.c displaySystemInfo函数中
void Modules_EXTI8_IRQHandler (void);						//模块插入到exti.c的PB8外部中断函数EXTI9_5_IRQHandler内，触发外部中断打断

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
