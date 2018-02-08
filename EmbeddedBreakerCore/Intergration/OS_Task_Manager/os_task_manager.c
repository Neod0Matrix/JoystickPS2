#include "Matrix.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//UCOS使用的任务管理器本地管理
//通过延时来完成任务的切换
//同时加入信号量和邮箱操作

OS_EVENT *msg_key;									//按键邮箱事件块指针
OS_EVENT *sem_led1;									//LED1信号量指针
OS_EVENT *sem_oled;									//OLED信号量指针

//任务堆栈定义，8字对齐
__align(8) static OS_STK START_TASK_STK[START_STK_SIZE];//起始任务堆栈
__align(8) static OS_STK LED1_Signal_Task_Stk[LED1_Signal_Stk_Size];//LED1信号量任务
__align(8) static OS_STK GPS_App_Task_Stk[GPS_App_Stk_Size];//GPS应用任务
__align(8) static OS_STK KeyBoard_Ack_Task_Stk[KeyBoardAck_Stk_Size];//键盘响应任务堆栈
__align(8) static OS_STK System_Detect_Task_Stk[System_Detect_Stk_Size];//系统状态监测任务堆栈
__align(8) static OS_STK Order_Responder_Task_Stk[Order_Responder_Stk_Size];//指令响应任务堆栈

//开始任务
void start_task (void *pdata)
{
    OS_CPU_SR cpu_sr = 0;							//临界区操作比含有该变量
    pdata = pdata;
    msg_key = OSMboxCreate((void *)0);				//创建消息邮箱
    sem_led1 = OSSemCreate(0);						//创建信号量
	sem_oled = OSSemCreate(0);
    OSStatInit();									//初始化统计任务，这里会延时1秒钟左右
    //创建任务过程不可被打断
    OS_ENTER_CRITICAL();							//进入临界区(无法被中断打断)
    //创建任务列表，嵌入函数名
    //任务创建序列：优先级从低到高
    OSTaskCreate(LED1_Signal_Task, (void *)0, (OS_STK*) & LED1_Signal_Task_Stk[LED1_Signal_Stk_Size - 1], LED1_Signal_Prio);//创建LED1任务
	OSTaskCreate(GPS_App_Task, (void *)0, (OS_STK*) & GPS_App_Task_Stk[GPS_App_Stk_Size - 1], GPS_App_Prio);//创建GPS任务
    OSTaskCreate(KeyBoardAck_Task, (void *)0, (OS_STK*) & KeyBoard_Ack_Task_Stk[KeyBoardAck_Stk_Size - 1], KeyBoardAck_Prio);//创建键盘任务
	OSTaskCreate(System_Detect_Task, (void *)0, (OS_STK*) & System_Detect_Task_Stk[System_Detect_Stk_Size - 1], System_Detect_Prio);//创建系统状态监测任务
    OSTaskCreate(Order_Responder_Task, (void *)0, (OS_STK*) & Order_Responder_Task_Stk[Order_Responder_Stk_Size - 1], Order_Responder_Prio);//创建指令响应任务
    OSTaskSuspend(START_TASK_PRIO);					//挂起起始任务，OS不再管理
    OS_EXIT_CRITICAL();								//退出临界区(可以被中断打断)
}

//UCOS工作预处理初始化
void uCOS_Task_Manager (void)
{
    OSInit();										//UCOS初始化
    OSTaskCreate(start_task, (void *)0, (OS_STK*) & START_TASK_STK[START_STK_SIZE - 1], START_TASK_PRIO);//创建起始任务
    OSStart();										//开启系统，函数在这里不会返回
}

//信号量处理
void Semaphore_Handler (void)
{
	u8 err;	
	u8 semmask = 0;
	switch ((u32)OSMboxPend(msg_key, 10, &err))		//来自键盘任务邮箱发送的消息
	{ 
		case KEY0_PRES: semmask = 1; OSSemPost(sem_led1); break;//发送led1信号量
		case KEY1_PRES: semmask = 1; OSSemPost(sem_oled); break;//发送oled信号量
		case WKUP_PRES: break;						//急停不参与
	}
	if (semmask || sem_led1 -> OSEventCnt || sem_oled -> OSEventCnt)//显示sem		
	{
		//区分信号量来源
		if (sem_led1 -> OSEventCnt)					
		{
			printf("\r\n[+]----------LED1 Message: %d----------[+]\r\n", sem_led1 -> OSEventCnt);//显示信号量的值
			while ((USART1 -> SR & 0X40) == 0);		//等待发送结束
		}
		else if (sem_oled -> OSEventCnt)
		{
			printf("\r\n[+]----------OLED Message: %d----------[+]\r\n", sem_oled -> OSEventCnt);//显示信号量的值
			while ((USART1 -> SR & 0X40) == 0);		//等待发送结束
		}
		if (sem_led1 -> OSEventCnt == 0 || sem_oled -> OSEventCnt == 0) semmask = 0;//停止更新
	} 
}

//LED1有两个任务
void LED1_Signal_Task (void *pdata)
{
    u8 err;
    OS_CPU_SR cpu_sr = 0;							//临界区操作比含有该变量
    while (1)
    {
		//信号量统计源于LED.c里的定时器中断
		if (LED1_blink_sem == 20)
			OSSemPend(sem_led1, 0, &err);			//LED1信号量
        OS_EXIT_CRITICAL();							//退出临界区(可以被中断打断)
        delay_ms(10);								//延时切换任务
    }
}

//GPS
void GPS_App_Task (void *pdata)
{
	//u8 err;
	OS_CPU_SR cpu_sr = 0;							//临界区操作比含有该变量
	while (1)
	{
		GPS_Application();							//GPS读取数据
		OS_EXIT_CRITICAL();							//退出临界区(可以被中断打断)
		//间隔释放CPU控制权
//		OSTaskSuspend(GPS_App_Prio);
//		OSTaskResume(GPS_App_Prio);
        delay_ms(10);								//延时切换任务
	}
}

//键盘响应
void KeyBoardAck_Task (void *pdata)
{
    OS_CPU_SR cpu_sr = 0;							//临界区操作比含有该变量
    pdata = pdata;
    while (1)
    {
		if (KEY_Scan(NotSupportLongPress)) 
			OSMboxPost(msg_key, (void*)KEY_Scan(NotSupportLongPress));//发送消息
        OS_EXIT_CRITICAL();							//退出临界区(可以被中断打断)
		//间隔释放CPU控制权
//		OSTaskSuspend(KeyBoardAck_Prio);
//		OSTaskResume(KeyBoardAck_Prio);
        delay_ms(10);								//延时切换任务
    }
}

//系统状态监测
void System_Detect_Task (void *pdata)
{
	OS_CPU_SR cpu_sr = 0;							//临界区操作比含有该变量
	while (1)
	{
		LVD_EW_Handler();							//输入电压低压监测
		ARM_Sensor_Detector();						//机械臂传感器限位检测
		OLED_Display_EW();							//显示错误警报
		OLED_Display_ITD();							//显示温度监测
		OLED_Display_CPU();							//显示CPU占用率
		Semaphore_Handler();						//信号量处理
		OS_EXIT_CRITICAL();							//退出临界区(可以被中断打断)
        delay_ms(10);								//延时切换任务
	}
}

//响应指令
void Order_Responder_Task (void *pdata)
{
	OS_CPU_SR cpu_sr = 0;							//临界区操作比含有该变量
    while (1)
    {
        Order_Responder();							//指令响应函数
		Emergency_StewHandler();					//急停处理
		OS_EXIT_CRITICAL();							//退出临界区(可以被中断打断)
        delay_ms(10);								//延时切换任务
    }
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
