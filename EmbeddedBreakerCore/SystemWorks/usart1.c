#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//================================================================================
//USART1相关配置
/*
	支持适应不同频率下的串口波特率设置
	加入了对printf的支持
	增加了串口接收命令功能
	修正了printf第一个字符丢失的bug
	修改串口初始化IO的bug
	修改了USART1_RX_STA,使得串口最大接收字节数为2的14次方
	增加了USART_REC_LEN,用于定义串口最大允许接收的字节数
	增加了对UCOSII的支持
*/

#if 1u
#pragma import(__use_no_semihosting)         

//标准库需要的支持函数                 
__packed struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit (int x) 										//添加了一个void
{ 
	x = x; 
} 

//重定义fputc函数 
//读stdin数据
int fputc (int ch, FILE *f)
{      
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
    USART_SendData(USART1, (uint8_t)ch);   
	return ch;
}

#endif 

/*
//如果使用MicroLib，fputc改为如下
int fputc (int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) 
	{
	}
    return ch;
}

int GetKey (void)  
{ 
    while (!(USART1 -> SR & USART_FLAG_RXNE));
    return ((int)(USART1 -> DR & 0x1FF));
}
*/
 
//如果使能了接收
#ifdef USART1_RX_EN  			 								
//串口1中断服务程序
//读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     						//接收缓冲，最大USART_REC_LEN个字节
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 						//发送缓冲,最大USART1_MAX_SEND_LEN字节
/*
	接收状态
	bit15，	接收完成标志
	bit14，	接收到0x0d
	bit13 ~ 0,接收到的有效字节数目
*/
u16 USART1_RX_STA = 0u;       								//接收状态标记
  
//初始化USART1
void USART1_Init (u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1 RCC
	
#ifdef USART1_Remap_Enable
	/*
		USART1重映射，使能端口复用和GPIOB
		必须先使能IO时钟再重映射才生效
		开启USART1端口重映射，将PA9 PA10换成PB6 PB7
	*/
	
	//USART1_TX PB6
	GPIO_Config_Init (	RCC_APB2Periph_GPIOB,			
						GPIO_Mode_AF_PP,					
						GPIO_Speed_50MHz,					
						GPIO_Remap_USART1,				//USART1重映射
						GPIO_Pin_6,					
						GPIOB,					
						NI,				
						EBO_Disable);
						
	//USART1_RX PB7
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOB,			
						GPIO_Mode_IN_FLOATING,					
						GPIO_Speed_50MHz,					
						GPIO_Remap_USART1,				//USART1重映射
						GPIO_Pin_7,					
						GPIOB,					
						NI,				
						EBO_Disable);
#else													//不开启重映射
	//USART1_TX PA9
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_AF_PP,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_9,					
						GPIOA,					
						NI,				
						EBO_Disable);
						
	//USART1_RX	PA10
	ucGPIO_Config_Init (RCC_APB2Periph_GPIOA,			
						GPIO_Mode_IN_FLOATING,					
						GPIO_Speed_50MHz,						
						GPIORemapSettingNULL,			
						GPIO_Pin_10,					
						GPIOA,					
						NI,				
						EBO_Disable);
#endif
	USART_DeInit(USART1); 									//复位串口1

	//USART1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//USART1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化NVIC寄存器
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;				//串口波特率，此处数值较大
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

	USART_Init(USART1, &USART_InitStructure); 				//初始化串口1
	
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 			//使能串口1的DMA发送
	DMA_ForUSART1_Accel();									//DMA1 CH4配置给USART1
	
	USART_Cmd(USART1, ENABLE);                				//使能串口1 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启串口接受中断
}

/*
	串口1中断服务程序
	如果把出口数据处理写在这里就是中断方法，写在外面就是查询方法
*/
void USART1_IRQHandler (void)                				
{
	u8 Res;
	
#if SYSTEM_SUPPORT_OS 										//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntEnter();    
#endif
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//接收中断(接收到的数据必须是0x0D 0x0A结尾)
	{
		Res = USART_ReceiveData(USART1);					//读取接收到的数据
		
		if ((USART1_RX_STA & 0x8000) == 0)					//接收未完成
		{
			if (USART1_RX_STA & 0x4000)						//接收到了0x0D
			{
				//接收错误,重新开始
				if (Res != 0x0A) 
					USART1_RX_STA = 0;	
				//接收完成了 
				else 
					USART1_RX_STA |= 0x8000;						
			}
			else 											//还没收到0X0D
			{	
				if (Res == 0x0D) 
					USART1_RX_STA |= 0x4000;
				else
				{
					//将串口读到的数据送入缓存数组，之后使用串口数据从这里提取(查询方式)
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = Res;
					//接收数据错误,重新开始接收	  
					if (++USART1_RX_STA > (USART1_REC_LEN - 1u)) 
						USART1_RX_STA = 0u;
				}		 
			}
		}													
    }														
	
#if SYSTEM_SUPPORT_OS 										//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
	OSIntExit();  											 
#endif
} 

//串口1,printf 函数
//确保一次发送数据不超过USART1_MAX_SEND_LEN字节
void u1_printf (char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART1_TX_BUF, fmt, ap);
    va_end(ap);
	
	//等待通道传输完成
    while (DMA_GetCurrDataCounter(DMA_USART1_TX_CHx) != 0)
	{
	}
	//通过dma发送出去
    start_OneDMAx_Transfer(DMA_USART1_TX_CHx, strlen((const char *)USART1_TX_BUF));
}

//等待USART1数据发送完毕
void usart1WaitForDataTransfer (void)
{
	//等待状态置位
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
	{
	}
}

#endif	

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
