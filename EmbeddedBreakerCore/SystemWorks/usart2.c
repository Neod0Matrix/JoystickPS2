#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	USART2 DMA相关配置
	初始化定时器4辅助USART2
	开启USART2的DMA通道
*/

#define U2AuxTimerx					TIM4				//定时器4
#define RCC_APB1Periph_TIMx			RCC_APB1Periph_TIM4	//定时器4总线时钟
#define Timerx_IRQn					TIM4_IRQn			//定时器4中断	
#define Timerx_TogglePeriod			99u					//定时器自动重装翻转周期
#define Timerx_Prescaler			7199u				//定时器分频器		
#define RCC_AxBPeriph_DMAx			RCC_AHBPeriph_DMA1	//DMA时钟设置

/*
	定时器TIM4作用：
	通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据
	如果2个字符接收间隔超过10ms，则认为不是1次连续数据，也就是超过10ms没有接收到
*/

//串口发送缓存区
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
#ifdef USART2_RX_EN   									//如果使能了接收   	  
//串口接收缓存区
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 					//接收缓冲,最大USART2_MAX_RECV_LEN个字节

/*
	任何数据,则表示此次接收完毕
	接收到的数据状态
	[15]:0,没有接收到数据;1,接收到了一批数据
	[14:0]:接收到的数据长度
*/
u16 USART2_RX_STA = 0u;

//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void USART2_Init (u32 baud)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
#ifdef USART2_Remap_Enable								//开启USART2端口重映射，将PA2 PA3换成PD5 PD6
	//USART2_TX   PD5
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOD,			
						GPIO_Mode_AF_PP,					
						GPIO_Speed_50MHz,					
						GPIO_Remap_USART2,				//USART2重映射
						GPIO_Pin_5,					
						GPIOD,					
						NI,				
						EBO_Disable);

	//USART2_RX	  PD6
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOD,			
						GPIO_Mode_IN_FLOATING,					
						GPIO_Input_Speed,				//无效参数				
						GPIO_Remap_USART2,				//USART2重映射
						GPIO_Pin_6,					
						GPIOD,					
						NI,				
						EBO_Disable);
#else													//原始配置PA2 PA3					
	//USART2_TX   PA2
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_AF_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_2,					
						GPIOA,					
						NI,				
						EBO_Disable);

	//USART2_RX	  PA3
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_IN_FLOATING,					
						GPIO_Input_Speed,				//无效参数					
						GPIORemapSettingNULL,			
						GPIO_Pin_3,					
						GPIOA,					
						NI,				
						EBO_Disable);
#endif
	USART_DeInit(USART2); 								//复位串口2

    USART_InitStructure.USART_BaudRate = baud;			//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(USART2, &USART_InitStructure); 			//初始化串口2
	
	//使能串口2的DMA发送
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  	
	DMA_ForUSART2_Accel();
	
    USART_Cmd(USART2, ENABLE);                    		//使能串口

#ifdef USART2_RX_EN		  								//如果使能了接收
    //使能接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		//开启中断

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//USART1优先级高于USART2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);						//根据指定的参数初始化VIC寄存器
	
	//GPS用USART2定时器4初始化，10ms定时
	TIM4_AuxUSART2_Init();		
	
    USART2_RX_STA = 0;									//清零
    TIM4_Switch(DISABLE);								//关闭TIM4
#endif
}

//USART2中断服务
void USART2_IRQHandler (void)
{
    u8 res;
	
#if SYSTEM_SUPPORT_OS 									//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
    {
        res = USART_ReceiveData(USART2);
        if (USART2_RX_STA < USART2_MAX_RECV_LEN)		//还可以接收数据
        {
            TIM_SetCounter(U2AuxTimerx, TimerInitCounterValue);				
            if (USART2_RX_STA == 0) 
				TIM4_Switch(ENABLE);					//使能定时器4的中断
			*(USART2_RX_BUF + (USART2_RX_STA++)) = res;	//记录接收到的值
        } 
		else
            USART2_RX_STA |= 1 << 15;					//强制标记接收完成
    }
	
#if SYSTEM_SUPPORT_OS 									//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf (char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART2_TX_BUF, fmt, ap);
    va_end(ap);
	
	//等待通道传输完成
    while (DMA_GetCurrDataCounter(DMA_USART2_TX_CHx) != 0)
	{
	}
	//通过dma发送出去
    start_OneDMAx_Transfer(DMA_USART2_TX_CHx, strlen((const char *)USART2_TX_BUF));
}

/*
	通用定时器中断初始化
	这里始终选择为APB1的2倍，而APB1为36M
	arr：自动重装值。
	psc：时钟预分频数
*/
void TIM4_AuxUSART2_Init (void)
{
	ucTimerx_InitSetting(	U2AuxTimerx, 
							Timerx_IRQn, 
							RCC_APB1Periph_TIMx, 
							TIMx_GPIO_Remap_NULL,
							Timerx_TogglePeriod, 
							Timerx_Prescaler, 
							TIM_CKD_DIV1, 
							TIM_CounterMode_Up, 
							irq_Use, 					
							0x03, 
							0x02, 
							DISABLE);					//暂且不使能
}

//设置TIM4的开关
void TIM4_Switch (FunctionalState status)
{
	TIM_SetCounter(U2AuxTimerx, TimerInitCounterValue);	//计数器清空
	TIM_Cmd(U2AuxTimerx, status);  						//使能TIMx
}

//定时器4中断服务程序
void TIM4_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 									//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
    if (TIM_GetITStatus(U2AuxTimerx, TIM_IT_Update) != RESET)//更新中断
    {
        USART2_RX_STA |= 1 << 15;						//标记接收完成
        TIM_ClearITPendingBit(U2AuxTimerx, TIM_IT_Update);//清除TIMx更新中断标志
        TIM4_Switch(DISABLE);							//关闭TIM4同时清空计数值
    }
	
#if SYSTEM_SUPPORT_OS 									//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}

#endif

//等待USART2数据发送完毕
void usart2WaitForDataTransfer (void)
{
	//等待状态置位
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET)
	{
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
