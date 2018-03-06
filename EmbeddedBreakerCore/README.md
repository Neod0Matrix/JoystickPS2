Copyright
===============

	Copyright @2017 T.WKVER </MATRIX>
	Code by </MATRIX>@Neod Anderjon(LeaderN)
	Thank to fork or watch my project

Update
===============

	最后更新时间: 2018/03/06pm1310
	详细更新内容见softlog.txt

Project Manual
==============

	</MATRIX>提供嵌入式开发维护支持
	通用嵌入式控制器底层框架最小内核 EmbeddedBreakerCore

	主控MCU：		STM32F103RET6
	芯片资源：		64脚芯片 256k flash 64k SRAM
	嵌入式系统：	μC/OS-III v3.03

	Frame name: 	EmbeddedBreakerCore
	Developer: 		Neod Anderjon 
	Organization: 	</MATRIX>
	Laboratory: 	T.WKVER
	OS_Version: 	μC/OS-III v3.03
	FWLib_Verson: 	v3.5
	Project_Verson: OS_v0p6_LTE 长期演进版(什么时候TMD才能跑出个stable来啊)_(:з」∠)_ _(┐「ε:)_
	
Use Manual
===============

	同一头文件包含头文件：stdafx.h Windows编程标准包含
	使用前请先打开config.c config.h文件按照注释提示修改系统资源配置
	之后详细阅读README.md，并且保证自己很清醒
	托管在Github上的是工程源码，如要使用在目录内添加MDK-ARMProject目录，在其中
	添加工程配置文件，建议使用Keil或者RealView环境
	根据实际芯片器件选择设置工程宏及编译优化选项
	define: STM32F10X_HD(芯片容量),USE_STDPERIPH_DRIVER(使用固件库)
	Optimize for time, One ELF Section per Function
	所有的模块链接到框架里的函数和设置项都在Modules/link.*里
	默认不开启OS，如需要修改config.*选项，并注意设置相应的任务特性
	请尽量不要修该框架内除link.*，config.*以外的文件内容
	移植框架后请失能MOE_Switch选项，详见config.c内容
	
Inner IO Map
===============

(标注有*表示该IO口在PCB上使用ULN2003A驱动，该芯片有反相特性)

	引脚	用途		I/O		备注

	PA0 ---- WKUPIO		I		
	PA1 ---- 		
	PA2/PB0  
	PA3/PB1  
	PA4 ---- 
	PA5 ---- 
	PA6 ---- 
	PA7 ---- 
	PA8 ---- LED0
	PA9/PB6  TXD		
	PA10/PB7 RXD		I
	PA11 --- 
	PA12 --- 
	PA13 --- SWDIO		I
	PA14 --- SWCLK
	PA15 --- KEY1		I	

	PB0/PA2  
	PB1/PA3  
	PB2 ----
	PB3 ---- 
	PB4 ---- 
	PB5 ----
	PB6 ----
	PB7 ----
	PB8 ---- STEWIO		I
	PB9 ----
	PB10 --- 
	PB11 --- 
	PB12 --- 
	PB13 --- OLED_SCL
	PB14 --- 
	PB15 --- OLED_SDA	I

	PC0 ---- AD1		I
	PC1 ---- AD2		I
	PC2 ---- AD3		I
	PC3 ---- AD4		I
	PC4 ---- BEEP
	PC5 ---- KEY0		I
	PC6 ---- 
	PC7 ---- 
	PC8 ---- 
	PC9 ---- 
	PC10 --- LVDIO		I
	PC11 --- MLED
	PC12 --- ALED
	PC13 --- 
	PC14 --- OSC
	PC15 --- OSC

	PD0 ---- OSC
	PD1 ---- OSC
	PD2 ---- LED1
	PD5/PA2  
	PD6/PA3  
	
