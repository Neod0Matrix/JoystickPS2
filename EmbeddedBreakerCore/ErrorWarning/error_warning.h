#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//错误报警机制
//报警对象：蜂鸣器，LED，串口，OLED，RTC

//声明各种错误类型
typedef enum
{
	Test_Occur		= 0,												//测试警报发生
    Error_Clear 	= 1,												//警报解除
    Emergency_Stop 	= 2,												//紧急停止
    SendData_Error 	= 3,												//串口数据发送错误
    PVD_Excess		= 4,												//STM32 PVD 低额报警
    Temp_Excess		= 5,												//MCU温度超额报警
    Hard_Fault		= 6,												//硬件错误
    LVD_Warn		= 7,												//24V输入电压低于18V
	Usage_Fault		= 8,												//用量错误
	Bus_Fault		= 9,												//总线错误
} globalSystem_EW;														//全局错误指示
extern globalSystem_EW Return_Error_Type;								//得到状态

extern globalSystem_EW detErrorStatus (void);
extern void EW_TriggerHandler (globalSystem_EW sys_ew);					//错误响应函数

//警报的手动控制
typedef enum
{
	open_ew = 0,
	close_ew = 1
} EWManCtrl;
#define ManualCtrlLimit			(EWManCtrl)2

extern void ManualCtrlEW (void);

//封装
#define TEST_OCCUR				EW_TriggerHandler(Test_Occur)			//警报发生

#define ERROR_CLEAR 			EW_TriggerHandler(Error_Clear)			//警报解除

#define EMERGENCYSTOP 			EW_TriggerHandler(Emergency_Stop) 		//紧急停止
#define EMERGENCYSTOP_16		ErrorWarning_Feedback(Emergency_Stop)

#define SERIALDATAERROR 		EW_TriggerHandler(SendData_Error) 		//串口数据发送错误
#define SERIALDATAERROR_16		ErrorWarning_Feedback(SendData_Error)

#define PVDVOLTAGEEXCESS		EW_TriggerHandler(PVD_Excess)			//PVD检测电压低额
#define PVDVOLTAGEEXCESS_16		ErrorWarning_Feedback(PVD_Excess)

#define TEMPERATUREEXCESS		EW_TriggerHandler(Temp_Excess)			//MCU内部温度监测
#define TEMPERATUREEXCESS_16	ErrorWarning_Feedback(Temp_Excess)

#define HARDFAULT				EW_TriggerHandler(Hard_Fault)			//硬件错误
#define HARDFAULT_16			ErrorWarning_Feedback(Hard_Fault)

#define LVDWARNING				EW_TriggerHandler(LVD_Warn)				//24V输入电压低于18V
#define LVDWARNING_16			ErrorWarning_Feedback(LVD_Warn)

#define USAGEFAULT				EW_TriggerHandler(Usage_Fault)			//用量错误
#define USAGEFAULT_16			ErrorWarning_Feedback(Usage_Fault)

#define BUSFAULT				EW_TriggerHandler(Bus_Fault)			//总线错误
#define BUSFAULT_16				ErrorWarning_Feedback(Bus_Fault)

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
