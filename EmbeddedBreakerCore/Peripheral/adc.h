#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//ADC库

//ADCx配置初始化
void ucADCx_Config (			uint32_t 		rcc_axbper_adc, 		
								ADC_TypeDef* 	adcx,
								uint32_t 		mode,
								FunctionalState scan_conv,							
								FunctionalState cont_conv,							
								uint32_t 		ext_tri_conv,
								uint32_t 		data_align,
								uint8_t 		ch_nbr);
//获得ADCx的通道值
uint16_t getADCxChannel_Data (	ADC_TypeDef* 	adcx, 
								uint8_t 		chx, 
								uint8_t 		rank, 
								uint8_t 		sample_period);

//ADC的多次取值求平均
u16 adcx_AverageHandler (		ADC_TypeDef* 	adcx,									
								uint8_t 		chx, 									
								uint8_t 		rank, 									
								uint8_t 		sample_period,							
								u8 				times,									
								u8				interval);	

//通道值获取，只取整数部分
//传参：通道编号，rank值(按通道数递增，范围1-16)
#ifndef adcChannelRankDefine
#define adcChannelRankDefine(ch, rk) (adcx_AverageHandler(	ADCx_Number, 		\
															ch, 				\
															rk, 				\
															ADC_SamplePeriod, 	\
															Average_Count, 		\
															ADC_SampleInterval) * (3.3 / 4096))	
#endif

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
