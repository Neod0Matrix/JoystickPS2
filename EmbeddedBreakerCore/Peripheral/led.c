#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//====================================================================================================
//初始化PA8 PD2 PC11 PC12作为LED IO口

//LED闪烁设置，单位us
#define BlinkInterval			500000u						//定时500ms翻转IO口
#define LED2BreathInterval		10000u						//LED2呼吸间隔
//随机闪烁LED
#define InvalMinLimit			10000u						//下限10ms
#define InvalMaxLimit 			300000u						//上限300ms

//结构体声明
BreathPWMGroup led2;																

//LED IO初始化
void LED_Init (void)
{
	//PA8 LED0 RLED
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_8,					
						GPIOA,					
						IHL,				
						EBO_Disable);
	//PD2 LED1 GLED
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOD,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_2,					
						GPIOD,					
						IHL,				
						EBO_Disable);
	//PC11 PC12 
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOC,			
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_11 | GPIO_Pin_12,					
						GPIOC,					
						IHL,				
						EBO_Disable);
	
	//呼吸灯参数初始化
	BreathPara_Init(&led2, LED2BreathInterval);
}

//LED集群动作控制
void LEDGroupCtrl (LEDGroupNbr nbr, LEDMoveList mv)
{
	/*
		用函数传参的方法封装不可调用的硬件IO状态操作
		也可以使用官方固件库里的GPIO函数操作电平
	*/
	switch (nbr)
	{
	case led_0:
		switch (mv)
		{
		case On: 	LED0_On; 	break;
		case Off: 	LED0_Off;	break;
		case Blink: LED0_Blink;	break;
		}
		break;
	case led_1:
		switch (mv)
		{
		case On: 	LED1_On; 	break;
		case Off: 	LED1_Off;	break;
		case Blink: LED1_Blink;	break;
		}
		break;
	case led_2:
		switch (mv)
		{
		case On: 	LED2_On; 	break;
		case Off: 	LED2_Off;	break;
		case Blink: LED2_Blink;	break;
		}
		break;
	case led_3:
		switch (mv)
		{
		case On: 	LED3_On; 	break;
		case Off: 	LED3_Off;	break;
		case Blink: LED3_Blink;	break;
		}
		break;
	}
}

//初始化完成闪烁标志
void Aft_PeriInit_Blink (void)
{
    u8 bb;													
	
    for (bb = 0; bb < 3; bb++)								//闪烁几次，直到有人发现已经完成初始化
    {
		LEDGroupCtrl(led_0, Blink);
        LEDGroupCtrl(led_1, Blink);
        delay_ms(50);
        LEDGroupCtrl(led_0, Blink);
        LEDGroupCtrl(led_1, Blink);
        delay_ms(50);
        LEDGroupCtrl(led_0, Blink);
        LEDGroupCtrl(led_1, Blink);
        delay_ms(50);
        LEDGroupCtrl(led_0, Blink);
        LEDGroupCtrl(led_1, Blink);
    }
}

//BlinkLED状态控制
void BlinkLED_StatusCtrl (void)
{
	static u16 runledBlinkSem = 0u;
	static u16 randledBlinkSem = 0u;
	static u32 randledinterval = InvalMinLimit;				//闪烁间隔变量
	
	//初始化过程常亮
	if (Return_Error_Type == Error_Clear && pwsf == JBoot)
	{
		LEDGroupCtrl(led_1, On);							//外设内定运行指示灯常亮
		LEDGroupCtrl(led_3, Off);
	}
	else if (Return_Error_Type == Error_Clear && pwsf != JBoot && globalSleepflag == SysOrdWork) 
	{
		if (runledBlinkSem++ == TickDivsIntervalus(BlinkInterval) - 1)
		{
			runledBlinkSem = 0u;
			LEDGroupCtrl(led_1, Blink);															
		}
		//可以关闭随机灯
		if (randledBlinkSem++ == TickDivsIntervalus(randledinterval) - 1 
			&& Light_Switch == Light_Enable)
		{
			randledBlinkSem = 0u;
			LEDGroupCtrl(led_3, Blink);	
			//更新随机闪烁间隔
			randledinterval = RangeRandom(InvalMinLimit, InvalMaxLimit);
		}
	}
}

/*
	呼吸灯结构体成员初始化
	传参：结构体指针，呼吸间隔
*/
void BreathPara_Init (BreathPWMGroup *led_nbr, u32 iv)
{
	led_nbr -> breathCtrlSem = 0u;
	led_nbr -> changeDirFlag = False;
	led_nbr -> dutyCycle = 0u;
	led_nbr -> breathInterval = iv;							//呼吸间隔单独初始化
}

/*
	呼吸灯效果生成
	传参：LED灯组编号，呼吸灯结构体
*/
void BreathPWMProduce (LEDGroupNbr nbr, BreathPWMGroup *led_nbr)
{
	//初始化过程关闭
	if (pwsf == JBoot || Return_Error_Type != Error_Clear)
		LEDGroupCtrl(nbr, Off);
	else if (Return_Error_Type == Error_Clear && pwsf != JBoot 
		&& globalSleepflag == SysOrdWork && Light_Switch == Light_Enable) 
	{
		//初始状态
		(led_nbr -> breathCtrlSem <= led_nbr -> dutyCycle)? \
			LEDGroupCtrl(nbr, On) : LEDGroupCtrl(nbr, Off);
		
		if (led_nbr -> breathCtrlSem++ == TickDivsIntervalus(led_nbr -> breathInterval) - 1)
		{
			led_nbr -> breathCtrlSem = 0u;					//信号量复位
			//注意，led_nbr -> dutyCycle无需复位
			if (!led_nbr -> changeDirFlag)
			{
				//占空比增大，灯逐渐变亮
				if (led_nbr -> dutyCycle++ == 99u) 			//占空比默认在0-100之间变化
					led_nbr -> changeDirFlag = True;		//换向
			}	
			else
			{
				//占空比减小，LED逐渐变暗
				if (led_nbr -> dutyCycle-- == 0u) 							
					led_nbr -> changeDirFlag = False;		//换向
			}
		}
	}
	//关闭呼吸灯
	else if (Light_Switch == Light_Disable)
	{
		//这里不作处理
	}
}

//呼吸灯组调用
void BreathLEDGroupCall (void)
{		
	BreathPWMProduce(led_2, &led2);
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
