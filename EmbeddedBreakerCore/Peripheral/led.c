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
Bool_ClassType StopLightEffect = False;						//停止LED特效标志
u8 gloablPWM_dutyCycle = 0u;								//全局PWM占空比

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
	
	//PA12 固定PWM发生测试IO
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,										
						GPIO_Mode_Out_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_12,							
						GPIOA,					
						IHL,				
						EBO_Enable);	
	
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
	case led_pwm:
		switch (mv)
		{
		case On: 	LED_PWM_On; break;
		case Off: 	LED_PWM_Off;break;
		case Blink:				break;						//PWM输出没有Blink选项意义	
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

//框架自带灯光特效失能后熄灭特效LED
void LightEffectDisable (void)
{
	//接收到失能信号并且处于特效灯未停止工作状态，关闭灯，标志置位
	if (Light_Switch == Light_Disable && StopLightEffect == False)
	{
		StopLightEffect = True;
		LEDGroupCtrl(led_1, Off);
		LEDGroupCtrl(led_2, Off);
		LEDGroupCtrl(led_3, Off);
	}
	//接收到使能信号并且灯关闭，复位标志
	else if (Light_Switch == Light_Enable && StopLightEffect == True)
	{
		StopLightEffect = False;
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
		//外设运行指示绿灯
		if (runledBlinkSem++ == TickDivsIntervalus(BlinkInterval) - 1)
		{
			runledBlinkSem = 0u;
			LEDGroupCtrl(led_1, Blink);															
		}
		//随机灯
		if (randledBlinkSem++ == TickDivsIntervalus(randledinterval) - 1 
			&& Light_Switch == Light_Enable)
		{
			randledBlinkSem = 0u;
			LEDGroupCtrl(led_3, Blink);	
			
			//更新随机闪烁间隔
			randledinterval = RangeRandom(InvalMinLimit, InvalMaxLimit);
		}
		//关闭随机灯
		else if (Light_Switch == Light_Disable)
		{
			//这里不作处理
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
	算法固定占空比发生器
	该算法经过测试在dutyRange设置为100，dutyRange每次递减1的情况下得到177.78Hz左右的频率
	若修改val(dutyRange)/delta(dutyRange)的比值就能对PWM波频率进行倍频或者分频
	但随之duty_cycle也要倍乘该系数才能符合协议设定值
	但PWM占空比的分辨率就变成了delta(dutyRange)
	总而言之不是个好方法
*/
void StablePWMProduce (u8 duty_cycle)
{
	static u16 dutyRange = 100u;
	
	//初始化过程关闭
	if (pwsf == JBoot || Return_Error_Type != Error_Clear)
		LEDGroupCtrl(led_pwm, Off);							//IO置高
	else if (Return_Error_Type == Error_Clear && pwsf != JBoot 
		&& globalSleepflag == SysOrdWork) 
	{
		//产生固定占空比的PWM波
		if (duty_cycle == dutyRange)
			LEDGroupCtrl(led_pwm, Off);						//IO置高
		if (dutyRange == 0u)
		{
			LEDGroupCtrl(led_pwm, On);						//IO置低
			dutyRange = 100u;
		}
		dutyRange -= 1u;
	}
}

//协议取得PWM占空比
void Get_pwmDutyCycle (void)
{
	u8 duty_cycle = (*(USART1_RX_BUF + PWM_DutyCycle_1Bit) * 10u 
		+ *(USART1_RX_BUF + PWM_DutyCycle_1Bit + 1));
	
	__ShellHeadSymbol__; U1SD("Get PWM Duty Cycle Value: %d\r\n", duty_cycle);
	
	gloablPWM_dutyCycle = duty_cycle;						//全局传参
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
		LEDGroupCtrl(nbr, 
			(led_nbr -> breathCtrlSem <= led_nbr -> dutyCycle)? On:Off);
		
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
	StablePWMProduce(gloablPWM_dutyCycle);
	LightEffectDisable();
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
