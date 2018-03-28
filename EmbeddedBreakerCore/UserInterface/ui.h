#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//user interface

//拼凑初始化logo字符串
#define CoreLogo 						(indStr_Catenate(indStr_Catenate(__Organization__, "@"), __AbbrFrameName__))

//UI切屏控制结构体
typedef __packed struct 
{
	u8 				ui_screen_nbr;		//切屏数
	u8 				ui_confirm_alter;	//确认切屏数
	unsigned long 	ui_fresh_cnt;		//刷新计数器
	Bool_ClassType	ui_alter_flag;		//自动/手动标志
} UI_ScreenAlterCtrl;
extern UI_ScreenAlterCtrl ui_oled;

//刷屏数常量值
void OLED_ScreenP0_Const (void);
void OLED_ScreenP1_Const (void);
void OLED_ScreenP2_Const (void);
void OLED_DisplayInitConst (void);		//显示的常量字符
//#define ScreenPageCount	4u				//页数统计

void UISAC_StructureInit (void);
void OLED_Display_RTC (void);			//显示RTC时间
void OLED_SleepStaticDisplay (void);	//睡眠状态
void OLED_StatusDetector (void);		//OLED状态显示检测
void OLED_PageAlterCtrl (void);
void UIScreen_DisplayHandler (void);

//框架内定义默认OLED切屏数
#define FrameDefault_oledScreen_Count	5u

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
