#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//定时器通用设置

//定时器初始化通用设置
//传参：定时器编号，定时器中断编号，RCC时钟总线，重映射，翻转周期ARR，分频器PSC，时钟分割，计数模式，中断开启标志，抢占优先级，子优先级，使能控制
void ucTimerx_InitSetting (	TIM_TypeDef 		*timx,				//定时器编号
							uint16_t 			tim_irqx,			//定时器中断编号
							uint32_t 			rcc_axbxperi_timx,	//RCC时钟总线
							uint32_t			remap,				//重映射
							uint16_t 			tim_arrtp, 			//翻转周期ARR
							uint16_t 			tim_psc, 			//分频器PSC
							uint16_t 			clkdv, 				//时钟分割
							uint16_t 			ctm,				//计数模式
							Is_Timerx_NeedIRQ 	irq_flag,			//中断开启标志
							uint8_t 			pp,					//抢占优先级
							uint8_t 			sp,					//子优先级
							FunctionalState 	control)  			//使能控制
{  
	//声明结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	NVIC_InitTypeDef 		NVIC_InitStructure;
	
	TIM_DeInit(timx);  												//复位设置
	
	//使能定时器对应总线时钟，高级定时器APB2 普通定时器APB1
	if (timx == TIM1 || timx == TIM8)
		RCC_APB2PeriphClockCmd(rcc_axbxperi_timx, ENABLE); 			
	else
		RCC_APB1PeriphClockCmd(rcc_axbxperi_timx, ENABLE); 			
	
	if (remap != TIMx_GPIO_Remap_NULL)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//重映射需要使能AFIO总线
		GPIO_PinRemapConfig(remap, ENABLE);							//重映射IO编号(重映射必须在RCC初始化后使用才有效)
	}
	
	TIM_TimeBaseStructure.TIM_Period = tim_arrtp;           		//翻转周期   
	TIM_TimeBaseStructure.TIM_Prescaler = tim_psc;  				//分频器
	
    TIM_TimeBaseStructure.TIM_ClockDivision = clkdv; 				//时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = ctm;					//向上计数模式
	
	//高级定时器的重复计数模式
	if (timx == TIM1 || timx == TIM8)
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			
	
    TIM_TimeBaseInit(timx, &TIM_TimeBaseStructure);  
	
	if (timx != TIM1 && timx != TIM8)
		TIM_SetCounter(timx, TimerInitCounterValue); 				//初始化时间清0  
	
	//以下为定时器中断设置
	if (irq_flag == irq_Use)
	{
		TIM_ClearFlag(timx, TIM_FLAG_Update);						//清理中断标志位
		
		TIM_ITConfig(  												//使能或者失能指定的TIMx中断
			timx, 											
			TIM_IT_Update |  										//TIM中断源
			TIM_IT_Trigger,   										//TIM触发中断源 
			ENABLE  												//使能
		);
		NVIC_InitStructure.NVIC_IRQChannel = tim_irqx;  			//TIMx中断编号
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pp;	//先占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = sp;			//从优先级
		//IRQ通道使能
		if (timx != TIM1 && timx != TIM8)
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		
		else
			NVIC_InitStructure.NVIC_IRQChannelCmd = control;		
		NVIC_Init(&NVIC_InitStructure);  							//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	}
	
	//配置完成后定时器使能，最后一道开关
	if (timx != TIM1 && timx != TIM8)
		TIM_Cmd(timx, control);  									
	else
		TIM_Cmd(timx, ENABLE);
}  

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
