#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//DMA1加速USART

//标准DMA配置函数
/*
	传参：DMA RCC时钟总线，DMA 通道编号，数据传输量，外设地址，存储器地址，传输方向，
	外设地址寄存器，内存地址寄存器，外设数据宽度，内存数据宽度，工作模式，通道优先级，DMA通道x内存到内存传输
*/
void ucDMAx_Config (uint32_t rcc_axbper_dma, 					//DMA RCC时钟总线
					DMA_Channel_TypeDef* chx, 					//DMA 通道编号
					uint16_t cndtr,								//数据传输量
					uint32_t cpar, 								//外设地址
					uint32_t cmar, 								//存储器地址
					uint32_t trans_dir, 						//传输方向
					uint32_t perinc,							//外设地址寄存器
					uint32_t meminc,							//内存地址寄存器
					uint32_t perds,								//外设数据宽度
					uint32_t memds,								//内存数据宽度
					uint32_t mode,								//工作模式
					uint32_t prio,								//通道优先级
					uint32_t m2m)								//DMA通道x内存到内存传输
{
	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(rcc_axbper_dma, ENABLE);				//使能DMA传输
	
    DMA_DeInit(chx);   											//将DMA的通道x寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  			//DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  				//DMA内存基地址
	DMA_InitStructure.DMA_DIR = trans_dir;  					//数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = cndtr;  					//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = perinc; 	 			//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = meminc;  					//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = perds;  			//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = memds; 				//数据宽度为8位
	DMA_InitStructure.DMA_Mode = mode;  						//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = prio; 						//DMA通道x拥有中优先级 
	DMA_InitStructure.DMA_M2M = m2m;  							//DMA通道x没有设置为内存到内存传输
	DMA_Init(chx, &DMA_InitStructure);  						//根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器  
}

//将DMA1的通道4作为USART1的加速通道
void DMA_ForUSART1_Accel (void)
{
	ucDMAx_Config(	RCC_AHBPeriph_DMA1, 
					DMA_USART1_TX_CHx, 
					0, 
					(uint32_t) & USART1 -> DR, 
					(uint32_t)USART1_TX_BUF, 					//USART1的缓存数组
					DMA_DIR_PeripheralDST,
					DMA_PeripheralInc_Disable,
					DMA_MemoryInc_Enable,
					DMA_PeripheralDataSize_Byte,
					DMA_MemoryDataSize_Byte,
					DMA_Mode_Normal,
					DMA_Priority_High,							//高优先级
					DMA_M2M_Disable);
}

//将DMA1的通道7作为USART2的加速通道
void DMA_ForUSART2_Accel (void)
{
	ucDMAx_Config(	RCC_AHBPeriph_DMA1, 
					DMA_USART2_TX_CHx, 
					0, 
					(uint32_t) & USART2 -> DR, 
					(uint32_t)USART2_TX_BUF, 					//USART2的缓存数组
					DMA_DIR_PeripheralDST,
					DMA_PeripheralInc_Disable,
					DMA_MemoryInc_Enable,
					DMA_PeripheralDataSize_Byte,
					DMA_MemoryDataSize_Byte,
					DMA_Mode_Normal,
					DMA_Priority_Medium,						//中优先级
					DMA_M2M_Disable);
}

//开启一次DMA传输
void start_OneDMAx_Transfer (DMA_Channel_TypeDef* chx, u8 len)
{ 
	DMA_Cmd(chx, DISABLE); 										//关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(chx, len);							//DMA通道的DMA缓存的大小
 	DMA_Cmd(chx, ENABLE);  										//使能USART1 TX DMA1 所指示的通道 
}	  

//USART使用DMA发送数据
//传参：DMA flag，通道编号
uint16_t USART_DMA_SendData (uint32_t umax_flag_tcx, DMA_Channel_TypeDef* chx, u8 len)
{	
	start_OneDMAx_Transfer(chx, len);							//开启DMA传输
	
	if (DMA_GetFlagStatus(umax_flag_tcx) != RESET)				//等待通道x传输完成
		DMA_ClearFlag(umax_flag_tcx);							//清除通道x传输完成标志
					
	return DMA_GetCurrDataCounter(chx);							//返回当前还剩余多少个数据		
}

/*
	使用DMA通道传输USART数据需要预先设定TX缓存数组
	将发送内容写入数组，再通过下面的函数发送
	如果只是用printf则不会走DMA
*/
void USART1_DMA_printf (void)
{
	USART_DMA_SendData(	DMAx_USART1_FLAG_TCx, 
						DMA_USART1_TX_CHx, 
						strlen((const char *)USART1_TX_BUF));	//设置到TX缓存上
}

//USART2的DMA printf
void USART2_DMA_printf (void)
{
	USART_DMA_SendData(	DMAx_USART2_FLAG_TCx, 
						DMA_USART2_TX_CHx, 
						strlen((const char *)USART2_TX_BUF));	//设置到TX缓存上
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
