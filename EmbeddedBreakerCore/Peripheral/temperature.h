#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//内部温度传感器

//获取温度ADC通道的计算值
#ifndef InnerTempCalcus
#define InnerTempCalcus(ch, rk) ((1.43f - adcChannelRankDefine(ch, rk)) / 0.0043f + 25.f)
#endif

extern float globalMCU_Temperature;
extern kf_1deriv_factor itd_kf;						//内部温度滤波器

void adcInnerTemperature_Init (void); 				//ADC通道初始化
void MCU_Temperature_Detector (void);				//计算出温度值

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
