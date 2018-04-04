#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//上位机通信协议

//添加通信协议栈
#define DH 					0xAA						//协议数据头部
#define DT					0xFF						//协议数据尾部
#define NB					0x00						//空位
#define SSR					0x0A						//系统状态请求
#define SOR					0x0B						//软件复位/软件睡眠
#define NQU					0x0C						//N皇后测试
//不能写0x0D，那是USART检测末尾字节的检测位
#define URC					0x0E						//URC的协议配置
#define MEW					0x0F						//警报的手动触发与清除

//数据位限定
#define DMAX				0x09						//十进制最大
#define DMIN				0x00						//十进制最小

//协议大小
#define Protocol_Stack_Size 18u							//协议栈单个元素大小
#define Stack_Member_Num	Get_Array_Size(Protocol_Stack)//协议栈元素个数

//协议参数位
#define Header_Bit			0u							//数据头位
#define Order_Type_Bit		1u							//算例编号位
#define Tail_Bit			(Protocol_Stack_Size - 1)	//数据尾位
#define EWF_Order_bit		2u							//报警反馈指令位
#define URC_SW_1Bit			2u							//取URC的开关标识1
#define URC_SW_2Bit			3u							//取URC的开关标识2
#define NQueen_1Bit			2u							//N皇后数据标识1
#define NQueen_2Bit			3u							//N皇后数据标识2
#define URC_ED_Bit			4u							//取URC的使能/失能标识
#define MEW_Read_Bit		2u							//取手动警报控制标识
#define SYS_RESLEEP_Bit		2u							//取软件复位/软件睡眠标识

//通过协议栈中的数据判断执行指令类型
//协议缓存数组第二位数据
typedef enum
{
    pSSR	= 0,
    pSOR	= 1,
	pNQU	= 2,
	pURC	= 3,
	pMEW	= 4,
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	pMDLS	= 5,
} Protocol_Order;
extern Protocol_Order PO_Judge;							//判断标识

//协议检查状态
typedef enum {pcl_pass = 1, pcl_error = !pcl_pass} pclShell_Status;				

//模仿linux shell界面做的交互头
#define __InterSymbol__		":~# "
#define __ShellHeadSymbol__ {U1SD("\r\n"); U1SD(__Frame_Name__ ); U1SD("@"); U1SD(__Organization__); U1SD(__InterSymbol__);}

pclShell_Status shellTrigger (void);					//协议栈检查、shell触发器
u8 ASCII_ToHexCode (u8 ascii);							//ASCII码转16进制码
u8 HexCode_ToASCII (u8 hexcode);						//16进制码转ascii
extern void NewCoor_ReqOrder (void);					//请求坐标
void CommunicationTest (void);							//通信测试
extern void Response_Strings (void);					//通信起始标志
extern void OrderResponse_Handler (void);				//指令响应器

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
