#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//系统底层功能配置

uint32_t rcc_main_freq;

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET (void)
{
	__ASM volatile ("wfi");		  
}

//关闭所有中断
void INTX_DISABLE (void)
{		  
	__ASM volatile ("cpsid i");
}

//开启所有中断
void INTX_ENABLE (void)
{
	__ASM volatile ("cpsie i");		  
}

//设置栈顶地址
//addr:栈顶地址
//MSP：主堆栈指针
__asm void MSR_MSP (u32 addr) 
{
    MSR MSP, r0 									//set Main Stack value
    BX r14
}

//软件复位
void Sys_Soft_Reset (void)
{  
    //SCB -> AIRCR = 0X05FA0000 | (u32) 0x04;		//摘自正点原子寄存器版本sys.c文件中使用的软件复位函数
	NVIC_SystemReset();								//core_cm3.h L1759中写入的软件复位函数
}  

//NVIC配置
void NVIC_Configuration (void)
{
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//默认使用分组2
}

/*
	RCC时钟配置，放在外设RCC时钟初始化之前
	V3.0版本固件库之前是必备的，V3.0之后使用缺省配置
*/
void RCC_Configuration (uint32_t pll)
{
    RCC_DeInit();              						//RCC寄存器恢复初始化值
    RCC_HSEConfig(RCC_HSE_ON); 						//使能外部高速晶振HSE
	
	//如果外部晶振启动成功，则进行下一步操作
    if (RCC_WaitForHSEStartUp() == SUCCESS)
    {
		/*
			AHB主要负责外部存储器时钟
			APB2负责AD，GPIO，高级TIM，USART1
			APB1负责DA，USB，SPI，I2C，CAN，USART2 USART3 UART4 UART5，普通TIM
			这里时钟选择为APB2的2倍，而APB2为36M，系统设置2倍频，TIM输入频率72Mhz
		*/
        //设置AHB时钟(HCLK)
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 			//RCC_SYSCLK_Div1——AHB时钟
        //注意此处的设置，如果使用SYSTICK做延时程序，此时SYSTICK(Cortex System timer)=HCLK/8=8MHz
        //设置低速AHB时钟(PCLK1)
        RCC_PCLK1Config(RCC_HCLK_Div2); 			//RCC_HCLK_Div2——APB1时钟
        //设置高速AHB时钟(PCLK2)
        RCC_PCLK2Config(RCC_HCLK_Div1); 			//RCC_HCLK_Div1——APB2时钟
		/*
			设置FLASH存储器延时时钟周期数
			FLASH时序延迟几个周期，等待总线同步操作
			推荐按照单片机系统运行频率
				0-24MHz时，取Latency=0
			   24-48MHz时，取Latency=1
			   48-72MHz时，取Latency=2
		*/
        FLASH_SetLatency(FLASH_Latency_2); 			//flash 2延时周期
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//预取指缓存使能
		
		//设置PLL时钟源及倍频系数，PLL的输入时钟=HSE时钟频率x倍频系数(2-16)
		switch (pll)
		{
		case 2: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2); break;
		case 3: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3); break;
		case 4: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4); break;
		case 5: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_5); break;
		case 6: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6); break;
		case 7: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_7); break;
		case 8: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_8); break;
		case 9: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); break;
		case 10: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_10); break;
		case 11: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_11); break;
		case 12: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12); break;
		case 13: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_13); break;
		case 14: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_14); break;
		case 15: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_15); break;
		case 16: RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_16); break;
		}
        RCC_PLLCmd(ENABLE);							//使能PLL
		
        //等待PLL准备完成
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);	//选择PLL作为时钟源
        //等待设置完成，返回时钟源标志
        while (RCC_GetSYSCLKSource() != PLLclk)		
        {
        }
    }
	//之后跟进RCC外设时钟的使能
}

//获取CPU主频
void Get_BSP_CPU_ClkFreq (void)            
{
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);               	//获取芯片的各种时钟频率
	rcc_main_freq = (uint32_t)rcc_clocks.HCLK_Frequency;//HCLK时钟频率
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
