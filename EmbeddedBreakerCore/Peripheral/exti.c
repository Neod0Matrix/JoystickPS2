#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//外部中断系统控制

//外部中断模式配置
//传参：IO口组，IO口源，外部中断总线，中断模式，触发方式，中断通道，抢占优先级，子优先级
void ucEXTI_ModeConfig (
						uint8_t 			io_group, 				//IO分组
						uint8_t 			io_src, 				//IO源
						uint32_t 			line, 					//总线
						EXTIMode_TypeDef 	it_mode, 				//中断模式
						EXTITrigger_TypeDef trigger,				//触发方式
						uint8_t 			irq,					//中断组
						uint8_t 			pp,						//抢占优先级
						uint8_t 			sp						//子优先级
					)
{
	EXTI_InitTypeDef EXTI_InitStructure;							//声明外部中断结构体组
	NVIC_InitTypeDef NVIC_InitStructure;							//声明中断向量结构体组

	EXTI_ClearITPendingBit(line);									//清理对应的总线
	GPIO_EXTILineConfig(io_group, io_src);							//配置IO
	
	EXTI_InitStructure.EXTI_Line = line;							//配置总线
	EXTI_InitStructure.EXTI_Mode = it_mode;							//中断模式
	EXTI_InitStructure.EXTI_Trigger = trigger;						//触发方式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//使能
	EXTI_Init(&EXTI_InitStructure);	 								//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = irq;						//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pp;		//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sp;				//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  							//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//外部中断初始化函数
void EXTI_Config_Init (void)
{
	//STEW(正常状态低电平，触发拉高) PB8	
	if (StewEXTI_Switch == StewEXTI_Enable)
	{
		//KEY0 PC5
		ucGPIO_Config_Init (RCC_APB2Periph_GPIOC,			
							GPIO_Mode_IPU,					
							GPIO_Input_Speed,						//无效参数						
							GPIORemapSettingNULL,							
							GPIO_Pin_5,					
							GPIOC,					
							NI,				
							EBO_Disable);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//外部中断，需要使能AFIO时钟
		ucEXTI_ModeConfig(	GPIO_PortSourceGPIOB, 
							GPIO_PinSource8, 
							Stew_EXTI_Line, 
							EXTI_Mode_Interrupt, 
							EXTI_Trigger_Rising, 
							EXTI9_5_IRQn, 
							0x01, 
							0x03);
	}
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			
	MPU6050_INT_IO_Init();
	ucEXTI_ModeConfig(	GPIO_PortSourceGPIOB, 
						GPIO_PinSource12, 
						MPU_INT_EXTI_Line, 
						EXTI_Mode_Interrupt, 
#if MPU_DataTransferFinishedINTLevel == Bit_RESET
						EXTI_Trigger_Falling, 						
#elif MPU_DataTransferFinishedINTLevel == Bit_SET
						EXTI_Trigger_Rising,
#endif
						EXTI15_10_IRQn, 
						0x03, 
						0x03);
*/
}

//STEW--PB8
void EXTI9_5_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 												//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntEnter();    
#endif
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	if (StewEXTI_Switch == StewEXTI_Enable && EXTI_GetITStatus(Stew_EXTI_Line) != RESET)//长按检测急停
	{
		EMERGENCYSTOP;												
		EMERGENCYSTOP_16;
		
		while (STEW_LTrigger);										//等待急停释放，允许长期检测
		ERROR_CLEAR;												//急停复位后自动清除警报	
	}
	EXTI_ClearITPendingBit(Stew_EXTI_Line);  						//清除EXTI线路挂起位
	
#if SYSTEM_SUPPORT_OS 												//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntExit();  											 
#endif
}

/*
	@EmbeddedBreakerCore Extern API Insert
*/
/*
void EXTI15_10_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 												
	OSIntEnter();    
#endif	
	*/

	/*	Here read one signal level not a key, handler method is different than last.
	 *	If you confirm test in external interrupt update dmp, please enable IMUINT_Enable.
	 *	Setting external interrupt may lead to program dump out, notice it.	
	**/
	/*
	dmpAttitudeAlgorithm_RT(IMUINT_Enable);
	EXTI_ClearITPendingBit(MPU_INT_EXTI_Line);  					//清除EXTI线路挂起位
	
#if SYSTEM_SUPPORT_OS 												
	OSIntExit();  											 
#endif
}
*/

//====================================================================================================
//code by </MATRIX>@Neod Anderjon

