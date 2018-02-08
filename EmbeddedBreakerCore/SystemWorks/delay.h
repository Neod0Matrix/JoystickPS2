#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//延时函数与芯片时钟RCC初始化

/*
	修正了中断中调用出现死循环的错误
	防止延时不准确,采用do while结构!
	增加了对UCOSII延时的支持.
	如果使用ucosII,delay_init会自动设置SYSTICK的值,使之与ucos的TICKS_PER_SEC对应
	delay_ms和delay_us也进行了针对ucos的改造.
	delay_us可以在ucos下使用,而且准确度很高,更重要的是没有占用额外的定时器.
	delay_ms在ucos下,可以当成OSTimeDly来用,在未启动ucos时,它采用delay_us实现,从而准确延时
	可以用来初始化外设,在启动了ucos之后delay_ms根据延时的长短,选择OSTimeDly实现或者delay_us实现.
	修改了使用ucos,但是ucos未启动的时候,delay_ms中中断无法响应的bug.
	在delay_us加入ucos上锁，防止由于ucos打断delay_us的执行，可能导致的延时不准。
	在delay_ms加入OSLockNesting判断
	修改OS支持方式,以支持任意OS(不限于UCOSII和UCOSIII,理论上任意OS都可以支持)
	添加:delay_osrunning/delay_ostickspersec/delay_osintnesting三个宏定义
	添加:delay_osschedlock/delay_osschedunlock/delay_ostimedly三个函数
	修正UCOSIII支持时的2个bug：
	delay_tickspersec改为：delay_ostickspersec
	delay_intnesting改为：delay_osintnesting
*/

void Delay_Init (void);

//如果SYSTEM_SUPPORT_OS为真，则需要支持OS
#if SYSTEM_SUPPORT_OS
//OS使用
void delay_us (u32 nus);
void delay_ms (u16 nms);
#else
//普通使用
void delay_us (u32 nus);
void delay_ms (u32 nms);
#endif

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
