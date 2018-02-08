#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//STM32低功耗睡眠模式

//简单的延时函数
void Delay (__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

//芯片睡眠模式
void mcu_SleepMode (void)
{
    __WFI();										//core_cm3.h内核asm定义
}

//芯片停止模式
void mcu_StopMode (void)
{
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//进入停止模式
}

//挂起处理
void mcu_SuspendHandler (void)
{
	__ShellHeadSymbol__; U1SD("Execute WFI\r\n");
	
    Sys_Standby();									//待命

    //等待中断，由于使用中断方式CPU不用轮询按键 
    while (1)
    {
        mcu_SleepMode();
        mcu_StopMode();
    }
}

//系统待命
void Sys_Standby (void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR外设时钟
	/**
    * @brief  Enables or disables the WakeUp Pin functionality
    * @param  NewState: new state of the WakeUp Pin functionality
    *   This parameter can be: ENABLE or DISABLE
    * @retval None
    */
    PWR_WakeUpPinCmd(ENABLE);  						//使能唤醒管脚功能
	/**
    * @brief  Enters STANDBY mode
    * @param  None
    * @retval None
    */
    PWR_EnterSTANDBYMode();	  						//进入待命（STANDBY）模式
}

//系统进入待机模式
void Sys_Enter_Standby (void)
{
    RCC_APB2PeriphResetCmd(0x01fc, DISABLE);		//复位所有IO口
    Sys_Standby();
}

//检测WKUP脚的信号
//返回值True：连续按下Ns以上
//      False：错误的触发
Bool_ClassType Check_WKUP (void)
{
    u8 pressTime = 0u;								//记录按下的时间
#define WaitTimeSec	3u								//等待时间，单位s
	
	while (True)
	{
		if (WK_UP_LTrigger && SM_Switch == SM_Enable)
		{
			pressTime++;							//已经按下了
			delay_ms(10 * WaitTimeSec);				
			//按下超过N秒钟
			if (pressTime >= 100u)
			{
				pressTime = 0u;						//时间清0
				
				return True; 						//按下Ns以上返回True，同时return跳出循环
			}
		} 
		else 
			return False; 							//按下不足N秒返回False，同时return跳出循环
	}
}

//PA0 WKUP唤醒初始化
void WKUP_Init (void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//使能复用功能时钟

    KEY_Init();										//PA0按键初始化
	
    //PA0 WKUP外部中断，必须配置成外部中断触发
	ucEXTI_ModeConfig(		GPIO_PortSourceGPIOA, 
							GPIO_PinSource0, 
							WKUP_EXTI_Line, 
							EXTI_Mode_Interrupt, 
							EXTI_Trigger_Rising, 
							EXTI0_IRQn, 
							0x03, 
							0x02);

	if (Stby_Switch	== Stby_Enable)					//开机待机功能使能
		if (Check_WKUP() == False) Sys_Standby();    
}

//中断,检测到PA0脚的一个上升沿
//中断线0线上的中断检测
void EXTI0_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 								//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntEnter();    
#endif
	
    if (Check_WKUP()) 
		Sys_Enter_Standby();						//WK_UP确认，系统进入待机模式
	EXTI_ClearITPendingBit(WKUP_EXTI_Line); 		//清除中断标志位
	
#if SYSTEM_SUPPORT_OS 								//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntExit();  											 
#endif
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
