#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//内部温度传感器，监测MCU工作温度
//不使用DS18B20外部传感器

float globalMCU_Temperature = 0.f;													//全局传参

#define ADCx_Number 				ADC1											//使用的ADC
#define RCC_AxBPeriph_ADCx			RCC_APB2Periph_ADC1								//ADC时钟
#define NumberOfADCChannel 			1u												//一共开启的ADC通道数目
#define ADC_SampleInterval			20u												//单次取值时间间隔，单位ms
#define ADC_SamplePeriod			ADC_SampleTime_239Cycles5						//ADC采样周期
#define Average_Count 				20u												//ADC取值求平均元素个数
#define ADC_Temp_Channel  			ADC_Channel_16 									//温度传感器通道
#define Warning_Temperature 		56.f											//报警温度(STM32原则上工作于-40~58度)

//初始化ADC 默认将开启通道0~3	
void adcInnerTemperature_Init (void)  
{	
	ucADCx_Config (	RCC_AxBPeriph_ADCx, 											//RCC时钟
					ADCx_Number,								
					ADC_Mode_Independent,											//ADC工作模式:ADC1和ADC2工作在独立模式
					DISABLE,														//模数转换工作在单通道模式
					DISABLE,														//模数转换工作在单次转换模式
					ADC_ExternalTrigConv_None,										//转换由软件而不是外部触发启动
					ADC_DataAlign_Right,											//ADC数据右对齐
					NumberOfADCChannel);	

	//温度ADC通道使能开关
	if (Temp_Switch == Temp_Enable)
	{
		ADC_TempSensorVrefintCmd(ENABLE); 											//开启内部温度传感器
		ADC_Cmd(ADCx_Number, ENABLE);												//使能指定的ADCx
	}
	else
	{
		ADC_TempSensorVrefintCmd(DISABLE); 											//关闭内部温度传感器
		ADC_Cmd(ADCx_Number, DISABLE);												//失能指定的ADCx
	}
}

//计算出内部温度值
void MCU_Temperature_Detector (void)
{
	static float act_temp = 0.f;													
	
	globalMCU_Temperature = InnerTempCalcus(ADC_Temp_Channel, 1);					
	if (act_temp != globalMCU_Temperature)	
	{		
		act_temp = globalMCU_Temperature;
		//内部温度检测主要工作：温度预警
		if (act_temp >= Warning_Temperature && pwsf != JBoot)
		{
			delay_ms(200);															//消除温度抖动引起的报警
			if (act_temp >= Warning_Temperature && pwsf != JBoot)
				TEMPERATUREEXCESS;															
		}
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
