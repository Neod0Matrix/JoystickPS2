#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//错误报警机制
//报警对象：蜂鸣器，LED，串口，OLED，RTC

globalSystem_EW Return_Error_Type = Error_Clear;									//返回一个错误状态

//全局返参
globalSystem_EW detErrorStatus (void)
{
	return Return_Error_Type;
}

//故障预警响应处理函数
void EW_TriggerHandler (globalSystem_EW sys_ew)										//函数调用传参，保证实时响应
{
    Return_Error_Type = sys_ew;														//全局传参
	
    if (EW_Switch == EW_Enable)
    {
		__ShellHeadSymbol__;
		if (sys_ew != Error_Clear)
		{
			Beep_Once;																//蜂鸣器哔一下														
			LEDGroupCtrl(led_0, On);												//开启故障指示灯
			LEDGroupCtrl(led_1, Off);												//关闭系统正常运行指示灯
			//关闭呼吸灯
			LEDGroupCtrl(led_2, Off);			
			LEDGroupCtrl(led_3, Off);
			
			U1SD("[!ERROR!!ERROR!] ");												//报错指示头部
			//选择错误类型打印报告log
			switch (sys_ew)														
			{
				//手动报警
			case Test_Occur: 	U1SD("Test Manual Warning\r\n");break;
				//紧急停止
			case Emergency_Stop: U1SD("Emergency Stop Press\r\n"); 					break;
				//发送串口数据错误
			case SendData_Error: U1SD("Receive Data Error With Protocol\r\n");		break;					
				//PVD检测STM32电源接口电压低额
			case PVD_Excess: 	U1SD("Controller Interface Voltage Error\r\n"); 	break;
				//温度超过阈值报警
			case Temp_Excess: 	U1SD("MCU Inner Temperature Excess\r\n"); 			break;			
				//硬件错误
			case Hard_Fault:
				if (SendDataCondition)
				{
					//fault状态寄存器(@0XE000ED28)包括:MMSR,BFSR,UFSR
					printf("@0XE000ED28 Status-Register Hardware Fault\r\n");
					usart1WaitForDataTransfer();
					//显示错误值
					printf("CFSR: 	%#8x\r\n", SCB -> CFSR);								
					usart1WaitForDataTransfer();		
					printf("HFSR: 	%#8x\r\n", SCB -> HFSR);									
					usart1WaitForDataTransfer();		
					printf("DFSR: 	%#8x\r\n", SCB -> DFSR);									
					usart1WaitForDataTransfer();		
					printf("AFSR: 	%#8x\r\n", SCB -> AFSR);									
					usart1WaitForDataTransfer();		
				} 																	break;
				//24V工业总线用电输入欠压报警
			case LVD_Warn: 		U1SD("Input Voltage Lower than 18V\r\n"); 			break;
				//系统用量错误
			case Usage_Fault: 	U1SD("MCU Usage Fault\r\n"); 						break;
				//总线错误
			case Bus_Fault: 	U1SD("MCU Bus Fault\r\n"); 							break;
				
				//故障清除，这里不做设置，只是应付switch-case的语法
			case Error_Clear: 														break;	
				
				//延时10ms切换任务
			default: delay_ms(10);	 												break;											
			}
		}
		//故障清除或者无错误
		else
		{
			LEDGroupCtrl(led_0, Off);												//关闭故障提示灯
			Beep_Once;	
			U1SD("[!CLEAR!!CLEAR!] No Error or Error Clear\r\n");
		}
		Beep_Once;																	//蜂鸣器最后哔一声
		
		//打印报错机制的激活时间，用于日志记录
		__ShellHeadSymbol__; U1SD("Log EW-Ruler Active Time\r\n");	
		RTC_ReqOrderHandler();														
    }
}

//手动控制警报系统
void ManualCtrlEW (void)
{
	EWManCtrl cmdType = (EWManCtrl)(USART1_RX_BUF[MEW_Read_Bit]);					//取标识数据
	
	//接收数据处理
	__ShellHeadSymbol__;
	if (cmdType < ManualCtrlLimit)	
	{
		if (SendDataCondition)
		{
			printf("Receive EW Command: [%d]\r\n", cmdType);
			usart1WaitForDataTransfer();
		}
		
		switch (cmdType)
		{
		case open_ew:	TEST_OCCUR;		break;
		case close_ew: 	ERROR_CLEAR;	break;
		}
	}
	//选项超值报错
	else 				
	{
		U1SD("Option Value Excess Error\r\n");			
		SERIALDATAERROR;		
	}
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
