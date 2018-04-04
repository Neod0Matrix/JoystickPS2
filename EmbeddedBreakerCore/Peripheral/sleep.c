#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//STM32低功耗睡眠模式

Is_SystemSleep globalSleepflag = SysOrdWork;

//系统待命
void Sys_Standby (void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR外设时钟
    PWR_WakeUpPinCmd(ENABLE);  						//使能唤醒管脚功能
    PWR_EnterSTANDBYMode();	  						//进入待命(STANDBY)模式
}

//系统进入待机模式
void Sys_Enter_Standby (void)
{
	LEDGroupCtrl(led_0, On);						//辅助查看LED
	globalSleepflag = SysIntoSleep;
	OLED_SleepStaticDisplay();
	Beep_Once;	
	delay_ms(50);
	Beep_Once;	
    RCC_APB2PeriphResetCmd(ALLIORESETORDER, DISABLE);//复位所有IO口
    Sys_Standby();
}

/*
	检测WKUP脚的信号，
	返回True: 连续按下3s以上
		False: 错误的触发	
*/
Bool_ClassType Check_WKUP (void) 
{
	u8 t = 0;					//记录按下的时间

	while (True)
	{
		if (WKUP_KD)
		{
			t++;				//已经按下了 
			LEDGroupCtrl(led_0, On);//辅助查看LED
			delay_ms(30);
			if (t >= 100)		
			{
				globalSleepflag = SysIntoSleep;
				//其他的任务在Sys_Enter_Standby()函数中完成
				
				return True; 	//按下3s以上了
			}
		}
		else 
		{
			globalSleepflag = SysOrdWork;
			LEDGroupCtrl(led_0, Off);
			
			return False; 		//按下不足3秒
		}
	}
}  

//PA0 WKUP唤醒初始化
void WKUP_Init (void)
{
	//WKUP PA0
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_IPD,					
						GPIO_Input_Speed,			//无效参数						
						GPIO_Remap_SWJ_JTAGDisable,								
						GPIO_Pin_0,					
						GPIOA,					
						NI,				
						EBO_Disable);
	
    //PA0 WKUP外部中断，必须配置成外部中断触发
	ucEXTI_ModeConfig(	GPIO_PortSourceGPIOA, 
						GPIO_PinSource0, 
						WKUP_EXTI_Line, 
						EXTI_Mode_Interrupt, 
						EXTI_Trigger_Rising, 
						EXTI0_IRQn, 
						0x02, 
						0x02);		
}

//协议指令控制
void Protocol_CtrlResetorSuspend (void)
{
	SYS_ResetorSleep rs_flag = (SYS_ResetorSleep)*(USART1_RX_BUF + SYS_RESLEEP_Bit);
	
	__ShellHeadSymbol__;
	switch (rs_flag)
	{
	case sys_reset: U1SD("Receive Option [0], System Reset");Sys_Soft_Reset(); 		break;
	case sys_sleep: U1SD("Receive Option [1], System Sleep");Sys_Enter_Standby();	break;
	}
}

//中断,检测到PA0脚的一个上升沿
void EXTI0_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 								//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntEnter();    
#endif
	
	EXTI_ClearITPendingBit(WKUP_EXTI_Line); 		//清除中断标志位
    if (Check_WKUP()) 
	{
		Sys_Enter_Standby();						//WK_UP确认，系统进入待机模式
		while (Check_WKUP());						//等待释放
	}
	
#if SYSTEM_SUPPORT_OS 								//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntExit();  											 
#endif
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
