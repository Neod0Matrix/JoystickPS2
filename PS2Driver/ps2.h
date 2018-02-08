#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//PS2手柄驱动

//DI->PB12 DO->PB1 CS->PC6 CLK->PC7
//以下接口都具有反相功能
#define DI   PBin(12)           //PB12  输入

#define DO_H PBout(1) = 0        	//命令位高
#define DO_L PBout(1) = 1        	//命令位低

#define CS_H PCout(6) = 0       //CS拉高
#define CS_L PCout(6) = 1       //CS拉低

#define CLK_H PCout(7) = 0      //时钟拉高
#define CLK_L PCout(7) = 1      //时钟拉低

//手柄键值映射数组
typedef enum
{
	ps2select 	= 1,
	ps2l3		= 2,
	ps2r3		= 3,
	ps2start	= 4,
	ps2padup	= 5,
	ps2padright = 6,
	ps2paddown  = 7,
	ps2padleft  = 8,
	ps2l2		= 9,
	ps2r2		= 10,
	ps2l1		= 11,
	ps2r1		= 12,
	ps2green	= 13,
	ps2red		= 14,
	ps2blue		= 15,
	ps2pink		= 16,
	ps2triangle	= 13,
	ps2circle	= 14,
	ps2cross	= 15,
	ps2square	= 26,
	ps2rx		= 5,
	ps2ry		= 6,
	ps2lx		= 7,
	ps2ly		= 8,
} StickKeyValueMap;
extern StickKeyValueMap kvm[16];

extern u8 Data[9];
extern u16 Handkey;

void PS2_Init(void);
u8 PS2_RedLight(void);//判断是否为红灯模式
void PS2_ReadData(void);
void PS2_Cmd(u8 CMD);		  //
u8 PS2_DataKey(void);		  //键值读取
u8 PS2_AnologData(StickKeyValueMap button); //得到一个摇杆的模拟量
void PS2_ClearData(void);	  //清除数据缓冲区
void PS2_ShortPoll (void);
void PS2_EnterConfig (void);
void PS2_TurnOnAnalogMode (void);
void PS2_VibrationMode (void);
void PS2_ExitConfig (void);
void PS2_SetInit (void);
void PS2_Vibration (u8 m1, u8 m2);
void PS2_Task (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
