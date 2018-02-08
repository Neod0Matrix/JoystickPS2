#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//初始化PC5，PA15，PA0作为测试按键，源于正点原子AlienTek MiniSTM32开发板设置

//定义硬件：按键输入端口
#define KEY0  					GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)	//读取按键0
#define KEY1  					GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)	//读取按键1
//#define WK_UP   				GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)	//读取WKUP
#define WK_UP 					PAin(0)			
#define STEW_IO					GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)	//读取急停

//定义按键键值映射
typedef enum 
{
	No_KEY_Map 	= 0,
	KEY0_PRES 	= 1,
	KEY1_PRES 	= 2,
	WKUP_PRES	= 3,
	STEW_PRES 	= 4,
} KeyBoard_MapTable;

//普遍输入检测连续取值
typedef enum {NSLP = 0, SLP = 1} Input_LoogPress;

void KEY_Init (void);														//IO初始化
KeyBoard_MapTable KEY_Scan (Input_LoogPress mode);  						//按键扫描函数

//按键短响应
//用于条件判断
//KEY0
#define KEY0_NLTrigger			(KEY_Scan(NSLP) == KEY0_PRES)
#define KEY0_LTrigger			(KEY_Scan(SLP) == KEY0_PRES)
//KEY1
#define KEY1_NLTrigger			(KEY_Scan(NSLP) == KEY1_PRES)
#define KEY1_LTrigger			(KEY_Scan(SLP) == KEY1_PRES)
//WK_UP
#define WK_UP_NLTrigger			(KEY_Scan(NSLP) == WKUP_PRES)
#define WK_UP_LTrigger			(KEY_Scan(SLP) == WKUP_PRES)
//STEW
#define STEW_NLTrigger			(KEY_Scan(NSLP) == STEW_PRES)
#define STEW_LTrigger			(KEY_Scan(SLP) == STEW_PRES)

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
