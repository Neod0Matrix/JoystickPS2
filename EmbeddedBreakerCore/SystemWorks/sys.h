#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//系统底层功能配置

//定义工程项目是否使用OS
#ifdef useRTOSinProject
#define SYSTEM_SUPPORT_OS		1							
#else
#define SYSTEM_SUPPORT_OS		0
#endif
																	    
//位带操作,实现51类似的GPIO控制功能
//具体实现思想，参考<<CM3权威指南>>第五章(87页~92页)
//IO口操作宏定义
#define BITBAND(addr, bitnum) 	((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2)) 
#define MEM_ADDR(addr)  		*((volatile unsigned long *)(addr)) 
#define BIT_ADDR(addr, bitnum)  MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    		(GPIOA_BASE + 12) 			//0x4001080C 
#define GPIOB_ODR_Addr    		(GPIOB_BASE + 12) 			//0x40010C0C 
#define GPIOC_ODR_Addr    		(GPIOC_BASE + 12) 			//0x4001100C 
#define GPIOD_ODR_Addr    		(GPIOD_BASE + 12) 			//0x4001140C 
#define GPIOE_ODR_Addr    		(GPIOE_BASE + 12) 			//0x4001180C 
#define GPIOF_ODR_Addr    		(GPIOF_BASE + 12) 			//0x40011A0C    
#define GPIOG_ODR_Addr    		(GPIOG_BASE + 12) 			//0x40011E0C    

#define GPIOA_IDR_Addr    		(GPIOA_BASE + 8) 			//0x40010808 
#define GPIOB_IDR_Addr    		(GPIOB_BASE + 8) 			//0x40010C08 
#define GPIOC_IDR_Addr    		(GPIOC_BASE + 8) 			//0x40011008 
#define GPIOD_IDR_Addr    		(GPIOD_BASE + 8) 			//0x40011408 
#define GPIOE_IDR_Addr    		(GPIOE_BASE + 8) 			//0x40011808 
#define GPIOF_IDR_Addr    		(GPIOF_BASE + 8) 			//0x40011A08 
#define GPIOG_IDR_Addr    		(GPIOG_BASE + 8) 			//0x40011E08 
 
//IO口操作
//不同的STM32F10x引脚数目不同，必先确保使用的芯片存在没有干扰的引脚才能初始化
//0<=n<=15
#define PAout(n)   				BIT_ADDR(GPIOA_ODR_Addr, n) //输出 
#define PAin(n)    				BIT_ADDR(GPIOA_IDR_Addr, n) //输入 

#define PBout(n)   				BIT_ADDR(GPIOB_ODR_Addr, n) //输出 
#define PBin(n)    				BIT_ADDR(GPIOB_IDR_Addr, n) //输入 

#define PCout(n)   				BIT_ADDR(GPIOC_ODR_Addr, n) //输出 
#define PCin(n)    				BIT_ADDR(GPIOC_IDR_Addr, n) //输入 

#define PDout(n)   				BIT_ADDR(GPIOD_ODR_Addr, n) //输出 
#define PDin(n)    				BIT_ADDR(GPIOD_IDR_Addr, n) //输入 

#define PEout(n)   				BIT_ADDR(GPIOE_ODR_Addr, n) //输出 
#define PEin(n)    				BIT_ADDR(GPIOE_IDR_Addr, n) //输入

#define PFout(n)   				BIT_ADDR(GPIOF_ODR_Addr, n) //输出 
#define PFin(n)    				BIT_ADDR(GPIOF_IDR_Addr, n) //输入

#define PGout(n)   				BIT_ADDR(GPIOG_ODR_Addr, n) //输出 
#define PGin(n)    				BIT_ADDR(GPIOG_IDR_Addr, n) //输入

//系统时钟源设置
typedef enum 
{	
	HSIclk = (uint8_t)0x00, 
	HSEclk = (uint8_t)0x04, 
	PLLclk = (uint8_t)0x08,
} SystemClock_Setting;

//以下为汇编函数
void WFI_SET (void);										//执行WFI指令
void INTX_DISABLE (void);									//关闭所有中断
void INTX_ENABLE (void);									//开启所有中断
void MSR_MSP (u32 addr);									//设置堆栈地址
				
//系统时钟优先级功能设置
void Sys_Soft_Reset (void);									//系统软件复位
void NVIC_Configuration (void);								//NVIC配置
void RCC_Configuration (uint32_t pll);						//RCC配置

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
