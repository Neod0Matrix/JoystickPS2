#include "stdafx.h"									
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	封装库顶层
	先进行控制器的底层外设初始化，再执行一系列应用操作，最后入栈
	坚持到重构就是胜利，去他娘的产品和测试
*/

PrgmaWork_Status pwsf;									//工作状态标志						

/*$PAGE*/
//外设初始化函数封装，好让做的工作看起来比原来多
void bspPeriSysCalls (void)
{
	//--------------------------------系统核心功能----------------------------------------//
	
	Delay_Init();	    	 							//延时函数，内部含有系统时钟初始化
    USART1_Init(USART1_Baud);	 						//串口1初始化，内含DMA通道配置
	USART2_Init(USART2_Baud);  							//串口2初始化，内含DMA通道配置和定时器4的初始化
	
	LED_Init();		  									//初始化与LED连接的硬件接口，先初始化它用作RTC等待显示
	
	RTC_Init_Check();									//RTC内部时钟初始化检查，这一步可能耗费较长时间
	PVD_Init_Config();									//初始化PVD，用于闹着玩的安全电压监测
	WKUP_Init();										//初始化WK_UP按键，同时检测是否正常开机
	
	Mem_Manager_Init();									//内存管理初始化

	//--------------------------定时器及外部中断设置初始化-------------------------------//
	
	TIM2_usTimeBase_Init(ENABLE);						//us级公交车定时器2初始化(就是谁都可以蹭个时基的意思)
	EXTI_Config_Init();									//外部中断初始化
	
	//----------------------------------IO口初始化----------------------------------------//
	
	KEY_Init();         								//初始化与按键连接的硬件接口
	Beep_IO_Init();										//初始化报警用蜂鸣器
	LVD_IO_Init();										//欠压检测IO
	
    //---------------------------------------ADC------------------------------------------//
	
	adcInnerTemperature_Init();							//初始化ADC内部温度传感器
    
	//--------------------------------界面功能初始化--------------------------------------//
	
    OLED_Init();										//初始化OLED
	
	/*
		@EmbeddedBreakerCore Extern API Insert
	*/
	Modules_HardwareInit();								//模块硬件初始化
}

/*$PAGE*/
//只对main.c有效 初始化底层硬件 
static void preSetUpHardware (void)
{
	RCC_Configuration(RCCMultipConst128M);				//RCC预设
	Get_BSP_CPU_ClkFreq();								//获取当前时钟主频，寄放到一个全局变量内
	pwsf = JBoot;										//系统刚启动
	
	NVIC_Configuration();								//设置中断优先级分组
	Universal_Resource_Config();						//统一资源配置器
    bspPeriSysCalls();									//初始化底层函数封装
	
	OLED_DisplayInitConst();							//UI初始化
	Response_Strings();									//与上位机通信的起始标志，表示控制器已初始化完成
}

/*$PAGE*/
//只对main.c有效，任务管理栈
static void stackTaskManager (void)
{
	//开始工作
	pwsf = StartW;										
	__ShellHeadSymbol__; U1SD("Step into Task Stack\r\n");
	
	//选择任务管理调度方式
	switch (TMMS)										
	{
	case RTOS: 		RTOS_TaskMgr(); 	break;			//实时任务操作系统
	case Streak:	Streak_TaskRun(); 	break;			//普通顺序调度任务	
	}	
}

/*
	采用标准系统传参：	参数数量(count)，
						参数值指针(value)，
						环境变量(env)
*/
int main (int argc, char *argv[], char *envp[])			
{
#ifdef DEBUG 											//debug/release
	debug();											
#endif
	
#ifdef MATRIX_fraExc_Lock								//框架执行锁匙(最高优先级指令)
    preSetUpHardware();									//启动底层硬件
	stackTaskManager();									//进入任务堆栈
#endif
	
	return (int)detErrorStatus();						//返回错误
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
