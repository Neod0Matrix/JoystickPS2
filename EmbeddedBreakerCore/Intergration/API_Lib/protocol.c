#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//机器视觉通信协议
//采用16进制数据进行通信

pclShell_Status order_bootflag;										//协议通过判断
Protocol_Order PO_Judge;											//指令类型判断标识

/*
	初始化二维数组作为协议栈
	确定协议长度，不确定协议个数
	栈值约束全部取最大值
	这是一个Stack_Member_Num x Protocol_Stack_Size大小的二维数组
*/
static int Protocol_Stack[][Protocol_Stack_Size] =
{
	//系统状态请求
    /*0*/ 	{DH, SSR, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//软件复位
    /*1*/	{DH, SOR, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//N皇后测试
	/*2*/	{DH, NQU, DMAX, DMAX, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//系统睡眠挂起
	/*3*/	{DH, MSS, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//通信测试
	/*3*/	{DH, CT, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//报警反馈
	/*4*/	{DH, EWF, CT, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//指令执行完成
	/*5*/	{DH, ORF, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//URC
	/*6*/	{DH, URC, NB, 0x03, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	//警报的手动触发与清除
	/*7*/	{DH, MEW, 0x01, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
};

//通信起始标志
void Response_Strings (void)
{
    LEDGroupCtrl(led_0, Off);										//关闭故障指示灯
    Beep_Off;														//正常情况下关闭
	Aft_PeriInit_Blink();
    displaySystemInfo();											//参看系统状态
    Beep_Once;														//初始化完成哔一声
}

//ASCII转16进制码
u8 ASCII_ToHexCode (u8 ascii) 
{
	u8 hexcode = 0u;
	
	if (ascii > 0u && ascii <= 9u) 									//0-9
		hexcode = ascii + 0x30;
	else if (ascii >= 10u && ascii <= 15u)							//A-F      
		hexcode = ascii + 0x41;
	else 
		hexcode = 0xff;     
	
	return hexcode; 
} 

//16进制码转ASCII
u8 HexCode_ToASCII (u8 hexcode) 
{  
	u8 ascii;
	
	if (hexcode >= 0x30 && hexcode <= 0x39) 
		ascii = hexcode - 0x30;
	else if (hexcode >= 0x41 && hexcode <= 0x46)					//大写字母  
		ascii = hexcode - 0x41; 									//0x37
	else if (hexcode >= 0x61 && hexcode <= 0x66)					//小写字母   
		ascii = hexcode - 0x61;										//0x57
	else   
		ascii = 0xff;  
	
	return ascii; 
} 

//发送报警信息
void ErrorWarning_Feedback (u16 order_src)
{
	u8 i = 0u;
	
	if (PC_Switch == PC_Enable && hexEW_Switch == hexEW_Enable)
	{
		__ShellHeadSymbol__; U1SD("EWF Message: ");
			
		Protocol_Stack[pEWF][EWF_Order_bit] = order_src;			//将对应的指令栈标志(16进制数据)压入协议栈
		
		//打印未使能也要发送16进制数据
		if (No_Data_Receive)
		{
			for (i = 0u; i < Protocol_Stack_Size; i++)					
			{
				//协议中对应的下位机发送请求
				printf("%02xdd", Protocol_Stack[pEWF][i]);			
				usart1WaitForDataTransfer();		
			}
			//自动换行
			U1SD("\r\n");
		}
		
		U1SD("\r\n");
	}
}

//指令执行结束
void orderExecuteFinished (void)
{
	u8 i = 0u;
	
	if (PC_Switch == PC_Enable && hexEW_Switch == hexEW_Enable)
	{
		__ShellHeadSymbol__; U1SD("Order Execute Finished: ");
			
		if (No_Data_Receive)
		{		
			for (i = 0u; i < Protocol_Stack_Size; i++)				//打印未使能也要发送16进制数据
			{
				printf("%02xdd", Protocol_Stack[pORF][i]);			//协议中对应的下位机发送请求
				usart1WaitForDataTransfer();		
			}
			//自动换行
			U1SD("\r\n");
		}
		
		U1SD("\r\n");
	}
}

//通信测试
void CommunicationTest (void)
{
	__ShellHeadSymbol__; U1SD("Communication Successful\r\n");
	//如果不成功则串口数据报错
}

/*
	协议检查指令匹配
	返回协议检查结果
	该函数与协议栈容量大小基本无关
*/
pclShell_Status shellTrigger (void)
{
    Protocol_Order pcl_match;										//匹配传参
	static Bool_ClassType memCreatedflag = False;					//内存创建标志
	
    order_bootflag = pcl_error;										//先关闭协议栈

#define Order_Num	Stack_Member_Num								//Stack_Member_Num也是指令类型数量
	
	//协议栈载入动态内存，只创建一次
	if (memCreatedflag == False)
	{
		stackOverFlow(Protocol_Stack);
		memCreatedflag = True;
	}
	
	//判断程序内部是否存在拒绝协议的指令设定
    if (PC_Switch == PC_Enable && Data_Receive_Over)				//串口1数据接收完成										
    {
		/*	
			遍历所有指令集栈
			Order_Num为指令类型数量，pcl_match只取数量-1
		*/
		for (pcl_match = pSSR; pcl_match <= Order_Num - 1u; pcl_match++)//pcl_match++自动循环
		{
			//只对三个数据标识进行检查，存在匹配漏洞，但是这样做比较简单不容易出错
			if (	
					//1、检查数据头部
					USART1_RX_BUF[Header_Bit] == Protocol_Stack[pcl_match][Header_Bit]
					//2、检查数据尾部(固有字节检查，也可以换成检查数据末尾字头)
					&& USART1_RX_BUF[Protocol_Stack_Size - 1u] 	== Protocol_Stack[pcl_match][Tail_Bit]
					//3、检查指令类型
					&& USART1_RX_BUF[Order_Type_Bit] == Protocol_Stack[pcl_match][Order_Type_Bit]
				)
			{
				PO_Judge = pcl_match;								//自动匹配，局部变量转全局变量
				order_bootflag = pcl_pass;							//协议认证通过
				
				//匹配到对应值后直接退出for循环，不再匹配剩下的指令		
				break;																
			}
			
			//数据公共鉴定位不正确
			//如果是匹配类型找不到，系统将不会做出任何反应
			else if (
					//数据头不正确
					USART1_RX_BUF[Header_Bit] 				!= Protocol_Stack[pcl_match][Header_Bit]
					//数据尾不正确
					|| USART1_RX_BUF[Protocol_Stack_Size - 1u] 	!= Protocol_Stack[pcl_match][Tail_Bit]
				)												
			{
				order_bootflag = pcl_error;							//不执行协议栈
				
				__ShellHeadSymbol__; U1SD("User Sent Data Error\r\n");
				SERIALDATAERROR;		
				SERIALDATAERROR_16;		
			}
        }
		
        if (order_bootflag == pcl_pass)								//最后协议通过，加载指令集shell	
		{			
			__ShellHeadSymbol__; U1SD("Protocol Created, Loading Shell\r\n");
			
			__ShellHeadSymbol__; U1SD("Log Order Execute Time\r\n");//打印指令执行时间，用于日志记录
			RTC_ReqOrderHandler();								
		}
		
		USART1_RX_STA = 0u;											//接收状态标记
    }
	
	//程序内部存在关闭协议栈的代码
    else if (PC_Switch == PC_Disable)															
    {
        order_bootflag = pcl_error;
		__ShellHeadSymbol__; U1SD("Sorry, Administrator Close Interface\r\n");
    }
	
    return order_bootflag;											//最后返回执行与否					
}

//协议指令响应调试
void OrderResponse_Handler (void)
{
	/*
		触发器先时检测，检测通过后才能执行协议指令
		安全及保密起见，shellTrigger()源码不能公开
	*/
    if (shellTrigger() == pcl_pass)									
    {
		Beep_Once;													//指令执行哔一声
		__ShellHeadSymbol__;
        switch (PO_Judge)											//后区分协议指令类型
        {
		case pCT:
			U1SD("Detect Communication Test Request\r\n");
			CommunicationTest();									//通信测试
			break;
		
		//-------------------------系统状态调试类指令-----------------------//
        case pSSR:
            displaySystemInfo();									//显示系统信息
            break;													
        case pSOR:
			U1SD("Software Control MCU Reset\r\n");
            Sys_Soft_Reset();										//软件复位
            break;		
		case pNQU:
			U1SD("N Queen Question MCU Ability Test\r\n");
            nQueen_CalculusHandler();								//N皇后测试
            break;
		case pMSS:
			U1SD("MCU Sleep/Suspend Test\r\n");
            mcu_SuspendHandler();									//系统睡眠处理
            break;
		case pURC:
			U1SD("Protocol Setting URC\r\n");
			pclURC_DebugHandler();									//协议配置资源									
			break;
		case pMEW:
			U1SD("Protocol Control Error-Warning\r\n");
			ManualCtrlEW();											//手动控制报警
			break;
		
		//--------------------------主动传输类，无操作----------------------//
		case pEWF:													//错误报警反馈
			break;
		case pORF:
			break;
		
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		case pSSDS:
			U1SD("MotorMotionControlModule API Single Step Debug Call\r\n");
			
			break;
        }
		
		Beep_Once;													//指令完成哔一声
		
		order_bootflag = pcl_error;									//判断条件之外依然处理成关闭或者error
		
		orderExecuteFinished();										//指令完成
		
		__ShellHeadSymbol__; U1SD("Log Order Execute Finished Time\r\n");//打印指令执行完成时间，用于日志记录
		RTC_ReqOrderHandler();
    }
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
