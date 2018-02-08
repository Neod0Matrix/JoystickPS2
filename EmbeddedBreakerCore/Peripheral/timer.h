#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//通用定时器设置

/*
	定时器普通应用计算：定时
	time(s) = (1 + Timerx_TogglePeriod) * (1 + Timerx_Prescaler) / 72M
	Timerx_TogglePeriod上限short
	Timerx_Prescaler上限short
*/
#ifndef TimeCalcusofucTimer
#define TimeCalcusofucTimer(a, p)		(((1 + a) * (1 + p))/ (Get_BSP_CPU_ClkFreq() / 1000000u))//单位us
#endif

#define TimerInitCounterValue			0u								//定时器计数初值
#define TIMx_GPIO_Remap_NULL			((uint32_t)0x00000000)			//定时器输出通道IO无重映射

//定义定时器是否需要开启中断
typedef enum {irq_Use = 1, irq_Unuse = !irq_Use} Is_Timerx_NeedIRQ;

//定时器通用初始化设置
extern void ucTimerx_InitSetting (	TIM_TypeDef 		*timx,			//定时器编号
									uint16_t 			tim_irqx,		//定时器中断编号
									uint32_t 			rcc_axbxperi_timx,//RCC时钟总线
									uint32_t			remap,			//重映射
									uint16_t 			tim_arrtp, 		//翻转周期ARR
									uint16_t 			tim_psc, 		//分频器PSC
									uint16_t 			clkdv, 			//时钟分割
									uint16_t 			ctm,			//计数模式
									Is_Timerx_NeedIRQ 	irq_flag,		//中断开启标志
									uint8_t 			pp,				//抢占优先级
									uint8_t 			sp,				//子优先级
									FunctionalState 	control);  		//使能控制

//====================================================================================================
//code by </MATRIX>@Neod Anderjon

