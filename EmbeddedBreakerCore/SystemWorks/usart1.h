#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//USART1相关配置，指令系统主要通信口
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

#define USART1_REC_LEN  	1024u  						//最大接收字节数 
#define USART1_MAX_SEND_LEN	1024u						//最大发送缓存字节数
#define USART1_Baud			115200u						//USART1波特率
	
#define Data_Receive_Over	(USART1_RX_STA & 0x8000)	//串口数据接收完成标志，Data_Receive_Over = 1表示接收完成，Data_Receive_Over = 0表示未完成
#define No_Data_Receive		(!Data_Receive_Over)		//无数据接收，可以发送
#define Data_All_Length 	(USART1_RX_STA & 0x3fff)	//数据总长
#define SendDataCondition	(PD_Switch == PD_Enable && No_Data_Receive)//发送数据条件
	  	
extern u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 			//发送缓冲,最大USART1_MAX_SEND_LEN字节
extern u8 USART1_RX_BUF[USART1_REC_LEN]; 				//接收缓冲,最大USART_REC_LEN个字节，末字节为换行符(不大于2的14次方 16384)
extern u16 USART1_RX_STA;         						//接收状态标记	

void USART1_Init (u32 bound);							//初始化USART1
void u1_printf (char* fmt, ...);						//USART printf
extern void usart1WaitForDataTransfer (void);			//等待数据发送完毕

//USART1普通打印变长参数
#ifndef U1SD
#define U1SD(format, ...) 	{if (SendDataCondition) {printf(format, ##__VA_ARGS__); \
							usart1WaitForDataTransfer();}}
#endif
//USART1报错打印变长参数，带文件行号
#ifndef U1SD_E
#define U1SD_E(format, ...)	{if (SendDataCondition) \
							{printf("FILE: "__FILE__", LINE: %d: "format"", __LINE__, ##__VA_ARGS__); \
							usart1WaitForDataTransfer();}}
#endif

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
