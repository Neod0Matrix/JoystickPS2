#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//PVD(Programmable Voltage Detector)STM32接口电源管理
//响应方式：调用Error_Warning报警

#define Trigger_Way			EXTI_Trigger_Falling		//电压从高电压下降到低于设定的电压阀值产生中断
#define Check_Voltage_Value	PWR_PVDLevel_2V8			//监控阈值
#define PVD_EXTI_Line		EXTI_Line16					//外部中断第16条总线，PVD
#define PVD_VT_Offset		0x00						//向量表偏移

//电源管理初始化
void PVD_Init_Config (void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //开启PVD时钟
	
	//PVD外部中断
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//外部中断，需要使能AFIO时钟
	EXTI_ClearITPendingBit(PVD_EXTI_Line); 				//清除中断
	
    EXTI_InitStructure.EXTI_Line = PVD_EXTI_Line;			
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = Trigger_Way;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
	
#ifdef  VECT_TAB_RAM
    NVIC_SetVectorTable(NVIC_VectTab_RAM, PVD_VT_Offset);
#else
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, PVD_VT_Offset);
#endif
	
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	//优先级要高于除外部中断的其他中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    //设定监控阀值
    PWR_PVDLevelConfig(Check_Voltage_Value);
	
    //使能开关API接口
    if (PVD_Switch == PVD_Enable) 
		PWR_PVDCmd(ENABLE);
    else 
		PWR_PVDCmd(DISABLE);
}

//PVD中断服务
void PVD_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS
    OSIntEnter();
#endif
	
    if (PWR_GetFlagStatus(PWR_FLAG_PVDO) == SET)
    {
        PVDVOLTAGEEXCESS;								//报警
		PVDVOLTAGEEXCESS_16;
    }
    EXTI_ClearITPendingBit(PVD_EXTI_Line); 				//清除中断
	
#if SYSTEM_SUPPORT_OS
    OSIntExit();
#endif
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
