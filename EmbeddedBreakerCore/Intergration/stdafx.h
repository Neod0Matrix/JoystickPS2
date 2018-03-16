#pragma once											//预编译一次
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	</MATRIX > 互联网+侦探组织提供嵌入式开发维护支持
	通用嵌入式控制器底层框架最小内核 EmbeddedBreakerCore
	stdafx.h Windows应用编程标准包含接口
	空定义头文件，用于包含所有封装库和官方库
*/

//keil提供的针对ARM做修改的库
#include <math.h>										//数学计算
#include <stdlib.h>										//取绝对值
#include <stdio.h>										//串口打印
#include <string.h>										//字符处理
#include <stdarg.h>										//参数宏
#include <inttypes.h>									//类型库

//系统常规运行库
#include "config.h"										//功能选择配置
#include "stm32f10x.h"									//STM32F10X系统文件，包含core_cm3.h
#include "sys.h"										//系统调用
#include "delay.h"										//延时函数
#include "usart1.h"										//USART1
#include "usart2.h"										//USART2

//API库
#include "main.h"										//配合main函数所在文件库
#include "protocol.h"									//通信协议
#include "sta_req.h"									//查看系统运行状态
#include "data.h"										//数据处理
#include "datascope_dp.h"								//串口虚拟示波器

//算法集合
#include "time_base.h"									//算法使用的时基
#include "pid.h"										//PID
#include "n_queen.h"									//N皇后CPU性能测试
#include "filter.h"										//数字滤波器

//系统功能监控机制
#include "error_warning.h"								//错误报警机制
#include "low_voltage.h"								//欠压保护监测
#include "pvd.h"										//PVD电源监测
#include "sleep.h"										//STM32睡眠模式
#include "temperature.h"								//内部温度传感器检测MCU温度
#include "rtc.h"										//STM32内部RTC

//OS运行库
#include "includes.h" 									//uC/OS包含库
#include "osiii_taskmgr.h"								//本地链接库
#include "local_taskmgr.h"								//任务管理器

//外设库
#include "gpio.h"										//GPIO通用初始化配置库
#include "timer.h"										//通用定时器配置
#include "exti.h"										//外部中断
#include "dma.h"										//DMA1加速USART1传输
#include "malloc.h"	    								//动态内存
#include "adc.h"										//ADC库
#include "i2c.h"										//I2C软件底层
#include "oled.h"										//初始化OLED
#include "ascii_font.h"  	 							//ASCII字库(OLED专用)

//IO口初始化
#include "led.h"										//LED IO口初始化
#include "key.h"										//键盘扫描IO口初始化
#include "beep.h"										//报警用蜂鸣器初始化

//UI
#include "ui.h"											//OLED UI sample

//模块
#include "link.h"										//模块对框架链接最末端文件

//===================================================================================================
//code by </MATRIX>@Neod Anderjon
