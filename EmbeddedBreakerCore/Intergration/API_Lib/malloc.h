#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//动态内存管理
 
#ifndef NULL
#define NULL 					0u								//内存常用NULL定义
#endif

//内存参数设定
#define MEM_BLOCK_SIZE			32u  	  						//内存块大小为32字节
#define MEM_MAX_SIZE			35u * 1024u  					//最大管理内存，单位字节 
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE / MEM_BLOCK_SIZE 	//内存表大小
 
//内存管理控制器
typedef __packed struct 
{
	void (*init)(void);											//初始化
	u8 (*perused)(void);		  								//内存使用率
	u8 	*membase;												//内存池 
	u16 *memmap; 												//内存管理状态表
	u8  memrdy; 												//内存管理是否就绪
} _m_mallco_dev;
extern _m_mallco_dev mallco_dev;							

//内部处理函数
void mymemset (void *s, u8 c,u32 count);						//设置内存
void mymemcpy (void *des, void *src, u32 n);					//复制内存     
void Mem_Manager_Init (void);					 				//内存管理初始化函数(外/内部调用)
u32 mem_malloc (u32 size);		 								//内存分配(内部调用)
u8 mem_free (u32 offset);		 								//内存释放(内部调用)
u8 mem_perused (void);											//得内存使用率(外/内部调用) 

extern u8 MemUsagePercent;										//全局内存占用率

//用户调用函数
void myfree (void *ptr);  										//内存释放(外部调用)
void *mymalloc (u32 size);										//内存分配(外部调用)
void *myrealloc (void *ptr, u32 size);							//重新分配内存(外部调用)		

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
