#pragma once   
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//DMA1加速USART

#define DMA_USART1_TX_CHx			DMA1_Channel4				//USART1 TX DMA 通道编号
#define DMA_USART2_TX_CHx			DMA1_Channel7				//USART2 TX DMA 通道编号
#define DMAx_USART1_FLAG_TCx		DMA1_FLAG_TC4				//USART1 DMA 通道标志
#define DMAx_USART2_FLAG_TCx		DMA1_FLAG_TC7				//USART2 DMA 通道标志

//标准DMA配置函数
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
					uint32_t m2m);								//DMA通道x内存到内存传输

void DMA_ForUSART1_Accel (void);
void DMA_ForUSART2_Accel (void);
void start_OneDMAx_Transfer (DMA_Channel_TypeDef* chx, u8 len);	//开启一次DMA传输
//实际应用到USART中
uint16_t USART_DMA_SendData (uint32_t umax_flag_tcx, DMA_Channel_TypeDef* chx, u8 len);
void USART1_DMA_printf (void);
void USART2_DMA_printf (void);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
