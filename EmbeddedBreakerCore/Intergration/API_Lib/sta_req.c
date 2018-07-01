#include "stdafx.h"									
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//打印系统请求状态

uint32_t devID[3];									//芯片设备ID号存储数组

//获取CPU唯一ID
void Get_MCU_SerialNumID (void) 
{ 
	//取相关寄存器值
	devID[0] = *(vu32 *)(0x1FFFF7F0);  				//高字节
	devID[1] = *(vu32 *)(0x1FFFF7EC); 
	devID[2] = *(vu32 *)(0x1FFFF7E8);				//低字节
	
	U1SD("\r\nChip SerialNum-ID: %#08X-%08X-%08X\r\n", devID[0], devID[1], devID[2]);	
} 

//预调功能算法测试接口(PTFAI)
void PreTesFuncAlgInterface (void)
{
	__ShellHeadSymbol__; U1SD("PTFAT Tested Finished\r\n");
}

//显示系统当前信息
void displaySystemInfo (void)
{
	//开发者信息
	__ShellHeadSymbol__; 
	U1SD("Developer Information Display:\r\n\
		\r\n%s [%s] Support\r\n\
		\r\nBased in STM32 Microchip Embedded Motion Controller(EMC)\r\n\
		\r\n%s Framework Support\r\n\
		\r\nCode by %s@%s\r\n\r\nIteration Version: %s\r\n", 
		__Laboratory__, __Logo__, __Frame_Name__, 
		__Organization__, __Developer__, __Code_Version__);
	
	//任务管理模式
	__ShellHeadSymbol__; U1SD("TaskManager Type: ");
#ifdef useRTOSinProject
	U1SD("RTOS %s Support\r\n", __OS_Version__); 
#else
	U1SD("Ordinary Streak TaskRun\r\n");	
#endif
	
	//CPU信息
	__ShellHeadSymbol__; 
	U1SD("CPU Resource Information:\r\n"); Get_MCU_SerialNumID();		//芯片ID
	U1SD("\r\nCPU Model: 		%s\r\n\
		\r\nCPU HCLK:		%dHz\r\n\
		\r\nCPU Flash: 		%sk\r\n\
		\r\nCPU SRAM: 		%sk\r\n", 
		__MCU_Model__, rcc_main_freq, 
		__MCU_Flash_Size__, __MCU_SRAM_Size__);						
	
	//系统状态信息
	__ShellHeadSymbol__; 
	U1SD("System Status Information:\r\n");	
	if (EW_Switch == EW_Enable)
	{
		U1SD("\r\nError Warning: 		%02d\r\n", Return_Error_Type);	//错误警告
	}
	if (Temp_Switch == Temp_Enable)
	{
		U1SD("\r\nMCU Temperature: 	%02.02f C\r\n", globalMCU_Temperature);//内部温度检测		
	}
#ifdef useRTOSinProject
	U1SD("\r\nRTOS CPU Usage: 	%.2f%%\r\n", (float)RTOSCPUUsage / 100);//CPU占用率		
#endif
	U1SD("\r\nMemory Usage: 		%d%%\r\n", mem_perused());			//显示内存使用率	
	
	if (pwsf == JBoot)
	{
		RTC_ReqOrderHandler();						//RTC时间
	}
	
	PreTesFuncAlgInterface();						//预调功能测试
	
	__ShellHeadSymbol__; 
	U1SD("Modules Inner Status: \r\n");
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	Modules_StatusReqHandler();						//模块内容打印
	
	__ShellHeadSymbol__; 
	U1SD("All Init Process Run Finished, You Can Start Your Work\r\n");
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
