#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//PS2手柄驱动

//DI/DATA->PB14 DO/CMD->PA1 CS->PA4 CLK->PA5
#define IO_DataIn   	PBin(14)         
#if defined use_ULN2003A
#define IO_Cmd_H 		(PAout(1) = Bit_RESET)        	
#define IO_Cmd_L 		(PAout(1) = Bit_SET)        	
#define IO_CS_H 		(PAout(4) = Bit_RESET)       
#define IO_CS_L 		(PAout(4) = Bit_SET)
#define IO_Clk_H 		(PAout(5) = Bit_RESET)
#define IO_Clk_L 		(PAout(5) = Bit_SET)      
#else
#define IO_Cmd_H 		(PAout(1) = Bit_SET)        	
#define IO_Cmd_L 		(PAout(1) = Bit_RESET)        	
#define IO_CS_H 		(PAout(4) = Bit_SET)       
#define IO_CS_L 		(PAout(4) = Bit_RESET)
#define IO_Clk_H 		(PAout(5) = Bit_SET)
#define IO_Clk_L 		(PAout(5) = Bit_RESET)   
#endif

//手柄键值映射数组
typedef enum
{
	ps2none		= 0,
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
	ps2triangle	= 13,
	ps2circle	= 14,
	ps2cross	= 15,
	ps2square	= 16,
} StickKeyValueMap;

//手柄摇杆映射数组
typedef enum 
{
	ps2rx		= 5,
	ps2ry		= 6,
	ps2lx		= 7,
	ps2ly		= 8,
} StickJoyValueMap;
extern u8 KeyValueCache[9];
extern StickKeyValueMap globalPS2keyValue;

static void PS2_SendCommand (u8 cmd);
Bool_ClassType PS2_RedLightMode (void);
static void PS2_ReadStickData (void);
static void PS2_ShortPoll (void);
static void PS2_EnterConfigMode (void);
static void PS2_TurnOnAnalogMode (void);
static void PS2_VibrationMode (void);
static void PS2_ExitConfigMode (void);
void PS2_InterfaceInit (void);
static void PS2_ModeConfigInit (void);
static void PS2_VibrationMotor (u8 m1, u8 m2);
StickKeyValueMap PS2_MatchStickKeyValue (void);
void OLED_DisplayPS2 (void);
void PS2_JoyStickResponseHandler (void);

//电机驱动
#define VibrateLeftMotor 	PS2_VibrationMotor(0x00, 0xff)		//左部电极震动
#define VibrateRightMotor 	PS2_VibrationMotor(0x00, 0xff)		//右部电极震动

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
