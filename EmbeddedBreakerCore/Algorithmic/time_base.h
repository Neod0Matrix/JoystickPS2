#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//算法使用的时基，公交车定时器(ಡωಡ)

//100us时基源
#define us_TogglePeriod			99u						//定时器自动重装翻转周期
#define us_Prescaler			71u						//定时器分频器	

//计算出目标定时需要的间隔，输入单位us，单个时间间隔必须大于定时时基，且为时基的整数倍
#ifndef TickDivsIntervalus
#define TickDivsIntervalus(us)	(us / (TimeCalcusofucTimer(us_TogglePeriod, us_Prescaler)))
#endif

void TIM2_usTimeBase_Init (FunctionalState control);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
