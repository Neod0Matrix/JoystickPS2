#ifndef __CAN_H__
#define __CAN_H__
#include "SYS.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//CAN总线

//CAN接收RX0中断使能
#define RX0_ENABLE			1
#define RX0_DISABLE			0
#define CAN_RX0_INT_ENABLE	RX0_DISABLE					//0,不使能;1,使能.								    

u8 CAN_Mode_Init (u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode);//CAN初始化
u8 Can_Send_Msg (u8* msg, u8 len);						//发送数据
u8 Can_Receive_Msg (u8 *buf);							//接收数据
void CAN1_Application (void);							//CAN应用

#endif

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
