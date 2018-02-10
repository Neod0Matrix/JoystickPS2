#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//USART2和DMA相关配置，配置给GPS接收数据

//USART2数据处理标志
#define USART2RecDataOver		USART2_RX_STA & 0X8000	//USART2数据接收完毕
#define USART2DataLength		USART2_RX_STA & 0X7FFF	//USART2数据总长

#define USART2_MAX_RECV_LEN		1024					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		1024					//最大发送缓存字节数		
#define USART2_Baud				38400					//GPS默认波特率38400

extern u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 			//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART2_RX_STA;   							//接收数据状态

void USART2_Init (u32 baud);							//串口2初始化
void u2_printf (char* fmt, ...);
void TIM4_Switch (FunctionalState status);				//定时器4开关
void TIM4_AuxUSART2_Init (void);						//初始化定时器4
extern void usart2WaitForDataTransfer (void);			//等待数据发送完毕

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
