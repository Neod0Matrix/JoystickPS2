#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	欠压保护监测
	初始化PC10作为监测IO口，低电平触发
	三极管放大电路正常状态下输出3.3v高电平，输入低于18v则变为低电平
*/

#define LV_TriggerLevel	0u														//触发电平

//输入检测IO口初始化
void LVD_IO_Init (void)
{
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOC,			
						GPIO_Mode_IPU,					
						GPIO_Input_Speed,										//无效参数					
						GPIORemapSettingNULL,			
						GPIO_Pin_10,					
						GPIOC,					
						NI,				
						EBO_Disable);
}

//欠压报警处理 如果对响应速度有要求可以考虑设置到外部中断
void LVD_EW_Handler (void)
{
    if (LVD_Switch == LVD_Enable)												//首先使能该功能
    {
		if (LVD_IO == LV_TriggerLevel)											//达到电平触发报警
			LVDWARNING;															//报警
    }
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
