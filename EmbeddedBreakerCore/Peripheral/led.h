#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//初始化PA8 PD2 PC11 PC12作为LED IO口

#define LED0 		PAout(8)			//PA8 	红灯 报警指示灯(初始化过程闪烁，正常状态关闭，报警时常亮)
#define LED1 		PDout(2)			//PD2 	绿灯 系统运行状态指示灯(初始化过程常亮，正常状态闪烁，报警时关闭)
#define LED2 		PCout(11)			//PC11 	蓝灯 呼吸灯(初始化过程关闭，正常状态呼吸，报警时关闭)
#define LED3		PCout(12)			//PC12 	蓝灯 随机灯(初始化过程关闭，正常状态随机闪烁，报警时关闭)

//LED共阳设计，低电平有效
typedef enum {LED_On = 0, LED_Off = !LED_On} LED_Status;

//封装
#define LED0_On 	(LED0 = LED_On)
#define LED0_Off	(LED0 = LED_Off)
#define LED0_Blink	(LED0 = !LED0)		
#define LED1_On 	(LED1 = LED_On)
#define LED1_Off	(LED1 = LED_Off)
#define LED1_Blink	(LED1 = !LED1)		
#define LED2_On		(LED2 = LED_On)
#define LED2_Off 	(LED2 = LED_Off)
#define LED2_Blink	(LED2 = !LED2)
#define LED3_On		(LED3 = LED_On)
#define LED3_Off	(LED3 = LED_Off)
#define LED3_Blink	(LED3 = !LED3)

//LED集群编号
typedef enum
{
	led_0 = 0,
	led_1 = 1,
	led_2 = 2,
	led_3 = 3,
} LEDGroupNbr;

//LED动作列表
typedef enum 
{
	On 		= 1,
	Off 	= 2,
	Blink 	= 3,
} LEDMoveList;

//呼吸灯算法结构体
typedef __packed struct 
{
	u16 			breathCtrlSem;		//计数信号量
	u16 			dutyCycle;			//占空比
	Bool_ClassType 	changeDirFlag;		//换向标识
	u32 			breathInterval;		//呼吸间隔
} BreathPWMGroup;
extern BreathPWMGroup led2;

void LED_Init (void);					//初始化
extern void LEDGroupCtrl (LEDGroupNbr nbr, LEDMoveList mv);
extern void Aft_PeriInit_Blink (void);	//系统外设初始化完成标志
extern void BlinkLED_StatusCtrl (void);	
void BreathPara_Init (BreathPWMGroup *led_nbr, u32 iv);
void BreathPWMProduce (LEDGroupNbr nbr, BreathPWMGroup *led_nbr);
extern void BreathLEDGroupCall (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
