#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//ADC库

//通用ADC初始化函数
void ucADCx_Config (			uint32_t 		rcc_axbper_adc, 					//RCC时钟
								ADC_TypeDef* 	adcx,								//ADCx
								uint32_t 		mode,								//工作模式
								FunctionalState scan_conv,							//通道扫描模式
								FunctionalState cont_conv,							//转换模式
								uint32_t 		ext_tri_conv,						//启动转换源
								uint32_t 		data_align,							//数据对齐
								uint8_t 		ch_nbr)								//该ADCx的通道数
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(rcc_axbper_adc, ENABLE);									//ADCx都在APB2总线上
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  												//设置ADC分频因子(6分频即72M/6=12M)
	
	ADC_DeInit(adcx);  																//复位ADC，将外设ADC的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = mode;												//ADC工作模式
	ADC_InitStructure.ADC_ScanConvMode = scan_conv;									//单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = cont_conv;							//单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ext_tri_conv;							//转换启动源
	ADC_InitStructure.ADC_DataAlign = data_align;									//ADC数据对齐
	ADC_InitStructure.ADC_NbrOfChannel = ch_nbr;									//ADC通道数
	ADC_Init(adcx, &ADC_InitStructure);												//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	ADC_Cmd(adcx, ENABLE);												
	
    ADC_ResetCalibration(adcx);														//使能复位校准
    while (ADC_GetResetCalibrationStatus(adcx));									//等待复位校准结束
    ADC_StartCalibration(adcx);	 													//开启AD校准
    while (ADC_GetCalibrationStatus(adcx));	 										//等待校准结束
}

//获得ADCx的某一通道值
uint16_t getADCxChannel_Data (	ADC_TypeDef* 	adcx, 								//ADCx
								uint8_t 		chx, 								//通道编号
								uint8_t 		rank, 								//定序组等级排名，与多通道采集有关
								uint8_t 		sample_period)						//采样周期
{
	ADC_RegularChannelConfig(adcx, chx, rank, sample_period);						
    ADC_SoftwareStartConvCmd(adcx, ENABLE);											//使能指定的ADCx的软件转换启动功能
	
	//等待转换结束
    while (!ADC_GetFlagStatus(adcx, ADC_FLAG_EOC))									
	{
	}
	
	return ADC_GetConversionValue(adcx);	
}

//多次采样取平均值，减小误差，非常重要
u16 adcx_AverageHandler (		ADC_TypeDef* 	adcx,								//ADCx
								uint8_t 		chx, 								//通道编号
								uint8_t 		rank, 								//定序组等级排名，与多通道采集有关
								uint8_t 		sample_period,						//采样周期
								u8 				times,								//检测次数取平均
								u8				interval)							//取值间隔
{
    u32 temp_val = 0u;
    u8 time = 0u;
	
    for (time = 0u; time < times; time++)											//多次取值
    {
        temp_val += getADCxChannel_Data(adcx, chx, rank, sample_period);
        delay_ms(interval);															//设置一次检测的时间												
    }
	
    return (temp_val / times);														//计算平均值
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
