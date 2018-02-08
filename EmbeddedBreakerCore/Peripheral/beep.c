#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//初始化PC4用于接入蜂鸣器			

//初始化蜂鸣器IO口
void Beep_IO_Init (void)
{
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOC,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_4,					
						GPIOC,					
						ILL,				
						EBO_Disable);
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
