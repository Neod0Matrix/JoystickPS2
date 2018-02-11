/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"										//官方库
#include "stdafx.h"												//本地库

void NMI_Handler (void)
{
}

/*
	硬件错误处理
	警戒堆栈溢出等问题
	如果报错太多没有解决就会引起栈溢出
	硬件报错是高级报错，如果其他报错不制止就会引发硬件报错
*/
void HardFault_Handler (void)	
{
    while (1)													//进入死循环，不能让程序继续运行
    {
        HARDFAULT;												//硬件报错	
		//如果错误直接触发复位
		if (HEDR_Switch == HEDR_Enable)
			Sys_Soft_Reset();										
		if (Return_Error_Type == Error_Clear) break;
    }
}

void MemManage_Handler (void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (True)
    {
		//如果错误直接触发复位
		if (HEDR_Switch == HEDR_Enable)
			Sys_Soft_Reset();										
		if (Return_Error_Type == Error_Clear) break;
    }
}


void BusFault_Handler (void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
		BUSFAULT;
		//如果错误直接触发复位
		if (HEDR_Switch == HEDR_Enable)
			Sys_Soft_Reset();	
		if (Return_Error_Type == Error_Clear) break;
    }
}

void UsageFault_Handler (void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
		USAGEFAULT;
		//如果错误直接触发复位
		if (HEDR_Switch == HEDR_Enable)
			Sys_Soft_Reset();	
		if (Return_Error_Type == Error_Clear) break;
    }
}

//请求管理调用(SerVers Call)
//__asm void SVC_Handler (void)
//{
//	MOVS 	r0, #4
//	MOV		r1, LR
//	TST 	r0, r1
//	BEQ		stacking_used_MSP
//	MRS		R0, PSP					
//	LDR		R1, = __cpp(SVC_Handler_Main)
//	BX		R1
//	stacking_used_MSP
//	MRS		R0, MSP
//	LDR		R1, = __cpp(SVC_Handler_Main)
//	BX		R1
//}

////SVC触发
//void SVC_Handler_Main (unsigned int *svc_args)
//{
//	switch (((char *)svc_args[6])[-2])
//	{
//		case 0:
//			svc_args[0] = svc_args[0] + svc_args[1];
//			break;
//		case 1:
//			svc_args[0] = svc_args[0] - svc_args[1];
//			break;
//		case 2:
//			svc_args[0] = svc_args[0] + 1;
//			break;
//		default://未知svc请求
//			break;
//	}
//	return;
//}

void DebugMon_Handler (void)
{
}

//排除OS影响	
//void PendSV_Handler (void)
//{
//}

//void SysTick_Handler (void)
//{
//}

//void SVC_Handler (void)
//{
//}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
