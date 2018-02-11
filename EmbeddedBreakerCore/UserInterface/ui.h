#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//user interface

//拼凑初始化logo字符串
#define CoreLogo 		(indStr_Catenate(indStr_Catenate(__Organization__, "@"), __AbbrFrameName__))

//刷屏设置	
extern u8 oledScreenFlag;

//刷屏数常量值
void OLED_ScreenP0_Const (void);
void OLED_ScreenP1_Const (void);
void OLED_ScreenP2_Const (void);	
void OLED_ScreenP3_Const (void);
void OLED_DisplayInitConst (void);		//显示的常量字符
//#define ScreenPageCount	4u				//页数统计

void OLED_Display_EW (void);			//显示error_warning
void OLED_Display_ITD (void);			//显示inner_temp_detector
void OLED_Display_CPU (void);			//显示OSCPU占用率
void OLED_Display_Mem (void);			//显示内存使用率
void OLED_Display_RTC (void);			//显示RTC时间
void OLED_SleepStaticDisplay (void);	//睡眠状态
void OLED_StatusDetector (void);		//OLED状态显示检测
void OLED_PageAlterCtrl (void);
void UIScreen_DisplayHandler (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
