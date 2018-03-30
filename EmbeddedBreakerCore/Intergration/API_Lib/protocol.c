#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//上位机通信协议
										
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
			//URC
	/*3*/	{DH, URC, NB, 0x03, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
			//警报的手动触发与清除
	/*4*/	{DH, MEW, 0x01, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, NB, DT},
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	/*5*/	Modules_Protocol,
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

//通信测试
void CommunicationTest (void)
{
	__ShellHeadSymbol__; U1SD("Communication Successful\r\n");
}

/*
	协议检查指令匹配
	返回协议检查结果
	该函数与协议栈容量大小基本无关
*/
pclShell_Status shellTrigger (void)
{
    Protocol_Order pcl_match;										//匹配传参
	pclShell_Status order_bootflag;									
	
	order_bootflag = pcl_error;										//先关闭协议栈认证
	//判断程序内部是否存在拒绝协议的指令设定
    if (PC_Switch == PC_Enable && Data_Receive_Over)				//串口1数据接收完成										
    {
		//遍历所有指令集栈
		for (pcl_match = (Protocol_Order)0; ; pcl_match++)			
		{
			/*
				只对三个数据标识进行检查，存在匹配漏洞
				但是这样做比较简单不容易报错
				你无法忍受一个嵌入式系统整天给你报错
			*/
			if (//1、检查数据头部
				*(USART1_RX_BUF + Header_Bit) == Protocol_Stack[pcl_match][Header_Bit]
				//2、检查数据尾部(固有字节检查，也可以换成检查数据末尾字头)
				&& *(USART1_RX_BUF + Tail_Bit) == Protocol_Stack[pcl_match][Tail_Bit]
				//3、检查指令类型
				&& *(USART1_RX_BUF + Order_Type_Bit) == Protocol_Stack[pcl_match][Order_Type_Bit])
			{
				//获得有效指令位
				order_bootflag = pcl_pass;			
				PO_Judge = pcl_match;				
								
				break;												
			}
			//数据公共鉴定位不正确
			else if (
				//数据头不正确
				*(USART1_RX_BUF + Header_Bit) != Protocol_Stack[pcl_match][Header_Bit]
				//数据尾不正确
				|| *(USART1_RX_BUF + Tail_Bit) != Protocol_Stack[pcl_match][Tail_Bit])												
			{
				order_bootflag = pcl_error;		
				PO_Judge = (Protocol_Order)Stack_Member_Num;		//限定位外值
				__ShellHeadSymbol__; U1SD("User Sent Data Header or Tail Error\r\n");	
				SERIALDATAERROR;	
			}
        }
			
		USART1_RX_STA = 0u;											//接收状态标记
    }
	
	//程序内部存在关闭协议栈的代码
    else if (PC_Switch == PC_Disable)															
    {
        order_bootflag = pcl_error;
		PO_Judge = (Protocol_Order)Stack_Member_Num;				//限定位外值
		__ShellHeadSymbol__; U1SD("Sorry, Developer Close Protocol Interface\r\n");
    }
	
    return order_bootflag;											//最后返回执行与否					
}

//协议指令响应调试
void OrderResponse_Handler (void)
{
    if (shellTrigger() == pcl_pass)									
    {
		__ShellHeadSymbol__;
        switch (PO_Judge)											//后区分协议指令类型
        {
		//-------------------------系统状态调试类指令-----------------------//
        case pSSR:
            displaySystemInfo();									//显示系统信息
            break;													
        case pSOR:
			U1SD("Software Control MCU Reset or Sleep\r\n");
			Protocol_CtrlResetorSuspend();							//复位or睡眠
            break;		
		case pNQU:
			U1SD("N Queen Question MCU Ability Test\r\n");
            nQueen_CalculusHandler();								//N皇后测试
            break;
		case pURC:
			U1SD("Protocol Setting URC\r\n");
			pclURC_DebugHandler();									//协议配置资源									
			break;
		case pMEW:
			U1SD("Protocol Control Error-Warning\r\n");
			ManualCtrlEW();											//手动控制报警
			break;
		
		/*
			@EmbeddedBreakerCore Extern API Insert
		*/
		case pMDLS:
			Modules_ProtocolTask();									
			break;
        }
		
		RTC_ReqOrderHandler();
		Beep_Once;													
    }
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
