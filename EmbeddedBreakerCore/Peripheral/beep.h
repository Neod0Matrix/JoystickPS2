#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//初始化PC4用于报警用蜂鸣器

#define Beep_IO 		PCout(4)							//PC4
#define Read_Beep_IO	PCin(4)								//读取延时较长的蜂鸣器

//高电平有效
typedef enum {WARNING = 1, SLIENCE = !WARNING} Beep_Status;	//定义enum型做蜂鸣器使能状态

//封装
#define Beep_On 		(Beep_IO = WARNING);
#define Beep_Off 		(Beep_IO = SLIENCE);
#define Beep_Once		{Beep_On; delay_ms(50); Beep_Off;}	//蜂鸣器哔一下

void Beep_IO_Init (void);									//蜂鸣器IO口初始化

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
