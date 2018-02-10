#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//按键初始化函数
//PA15和PC5 设置成输入

//一般输入检测IO口初始化
void KEY_Init (void)
{
	//一般设置成上拉输入
	
	//KEY0 PC5
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,			
						GPIO_Mode_IPU,					
						GPIO_Input_Speed,										//无效参数						
						GPIO_Remap_SWJ_JTAGDisable,								//关闭jtag，启用swd
						GPIO_Pin_5,					
						GPIOC,					
						NI,				
						EBO_Disable);
	//KEY1 PA15	
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_IPU,					
						GPIO_Input_Speed,										//无效参数						
						GPIO_Remap_SWJ_JTAGDisable,								//关闭jtag，启用swd
						GPIO_Pin_15,					
						GPIOA,					
						NI,				
						EBO_Disable);
	
}

//按键映射扫描，本函数不带有按键释放检查功能
KeyBoard_MapTable KEY_Scan (Input_LoogPress mode)
{
    Bool_ClassType slPressFlag = True;											//按键按松开标志
	
    if (mode) slPressFlag = True;  							
	
    if (slPressFlag && (KEY0 == 0 
					|| 	KEY1 == 0 
#ifdef StewLowLevelTrigger
					|| 	STEW_IO == 0)	
#else
					|| 	STEW_IO == 1)
#endif	
		)
    {
		Beep_Once;																//蜂鸣器内部本身有50ms延时
        slPressFlag = False;
		
		//返回映射键值
		//下降沿有效，拉低使能
        if (KEY0 == 0) 			return KEY0_PRES;		
        else if (KEY1 == 0) 	return KEY1_PRES;
#ifdef StewLowLevelTrigger
		else if (STEW_IO == 0) 	return STEW_PRES;
#else
		else if (STEW_IO == 1) 	return STEW_PRES;
#endif
    }
    else if (	KEY0 == 1 
			&& 	KEY1 == 1 
#ifdef StewLowLevelTrigger
			&& 	STEW_IO == 1)
#else
			&& 	STEW_IO == 0)
#endif
        slPressFlag = True;
	
    return No_KEY_Map;															//无按键按下
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
