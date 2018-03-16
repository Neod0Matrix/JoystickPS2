#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//内部温度传感器
   
#define TEMPCONST25			1.43f					//25度时的温度系数
#define PERCENTCONST		0.0043f					//倍率系数
#define BASETEMP25			25.f					//基础温度25度

//获取温度ADC通道的计算值
#ifndef InnerTempCalcus
#define InnerTempCalcus(ch, rk) ((TEMPCONST25 - adcChannelRankDefine(ch, rk)) / PERCENTCONST + BASETEMP25)
#endif

extern kf_1deriv_factor itd_kf;						//内部温度滤波器
extern float globalMCU_Temperature;

void adcInnerTemperature_Init (void); 				//ADC通道初始化
void MCU_Temperature_Detector (void);				//计算出温度值

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
