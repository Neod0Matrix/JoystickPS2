#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*	
	uC/OS-III本地配置时间片轮调度
	内存管理、共享资源
	框架倒数第二层封装
*/
//共享资源区
#define SR_Space				100u				//共享资源区空间(数组大小)
#define SRpTime					20u					//共享资源处理时间，单位ms
#define MemBlockSize			sizeof(int) * 10u	//由于一个指针变量占用4字节所以块的大小一定要为4的倍数(大于4)

/*$PAGE*/
#define Align_Byte				sizeof(u32) * 2u	//堆栈unsigned int 8字对齐
//任务堆栈定义，Align_Byte字对齐
__align(Align_Byte) static CPU_STK divTaskSStk[DivTaskSStkSize];//起始任务堆栈
__align(Align_Byte) static CPU_STK divTask1Stk[DivTask1StkSize];
__align(Align_Byte) static CPU_STK divTask2Stk[DivTask2StkSize];
__align(Align_Byte) static CPU_STK divTask3Stk[DivTask3StkSize];
__align(Align_Byte) static CPU_STK divTask4Stk[DivTask4StkSize];

//任务控制块
OS_TCB startTaskTCB;								//起始任务控制块
OS_TCB divTask1TCB;									
OS_TCB divTask2TCB;									
OS_TCB divTask3TCB;									
OS_TCB divTask4TCB;									

u8 share_resource[SR_Space];   						//共享资源区

//内存管理使用
OS_MEM osMemStorageArea;							//定义一个存储区
//存储区的内存池，使用内部RAM
CPU_INT08U osMemStoragePool[TaskStack_Nbr][MemBlockSize];

/*$PAGE*/
//开始任务
void divTaskNbrS (void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;									//参数传递指针
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  					//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN						//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  					//当使用时间片轮转的时候
	//使能时间片轮转调度功能，时间片长度为1个系统时钟节拍，既1 * 5 = 5ms
	OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err);  
#endif		
		
	OS_CRITICAL_ENTER();							//进入临界区
	
    /*
		创建任务列表，嵌入函数名
		任务创建序列：优先级从低到高
		创建内存块检查任务
	*/
	OSTaskCreate((OS_TCB*     )&divTask4TCB,		
				 (CPU_CHAR*   )"task 4", 		
                 (OS_TASK_PTR )divTaskNbr4, 			
                 (void*       )0,					
                 (OS_PRIO	  )DivTask4Prio,     
                 (CPU_STK*    )&divTask4Stk[0],	
                 (CPU_STK_SIZE)DivTask4StkSize / 10,	
                 (CPU_STK_SIZE)DivTask4StkSize,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
				 
	OSTaskCreate((OS_TCB*     )&divTask3TCB,		
				 (CPU_CHAR*   )"task 3", 		
                 (OS_TASK_PTR )divTaskNbr3, 			
                 (void*       )0,					
                 (OS_PRIO	  )DivTask3Prio,     
                 (CPU_STK*    )&divTask3Stk[0],	
                 (CPU_STK_SIZE)DivTask3StkSize / 10,	
                 (CPU_STK_SIZE)DivTask3StkSize,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);	
				 
	OSTaskCreate((OS_TCB*     )&divTask2TCB,		
				 (CPU_CHAR*   )"task 2", 		
                 (OS_TASK_PTR )divTaskNbr2, 			
                 (void*       )0,					
                 (OS_PRIO	  )DivTask2Prio,     
                 (CPU_STK*    )&divTask2Stk[0],	
                 (CPU_STK_SIZE)DivTask2StkSize / 10,	
                 (CPU_STK_SIZE)DivTask2StkSize,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
				 
	OSTaskCreate((OS_TCB*     )&divTask1TCB,		
				 (CPU_CHAR*   )"task 1", 		
                 (OS_TASK_PTR )divTaskNbr1, 			
                 (void*       )0,					
                 (OS_PRIO	  )DivTask1Prio,     
                 (CPU_STK*    )&divTask1Stk[0],	
                 (CPU_STK_SIZE)DivTask1StkSize / 10,	
                 (CPU_STK_SIZE)DivTask1StkSize,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
				 
    OS_CRITICAL_EXIT();								//退出临界区
	OSTaskDel((OS_TCB *)0, &err);					//删除startTask任务自身
}

//UCOS工作预处理初始化
void ucosiii_TaskMgr (void)
{
	OS_ERR err;
	
	CPU_SR_ALLOC();
    OSInit(&err);		    						//初始化OS
	OS_CRITICAL_ENTER();							//进入临界区	
	
	//创建一个存储分区
	OSMemCreate((OS_MEM*	)&osMemStorageArea,
				(CPU_CHAR*	)"internal mem",		
				(void*		)&osMemStoragePool[0][0],
				(OS_MEM_QTY	)TaskStack_Nbr,
				(OS_MEM_SIZE)MemBlockSize,
				(OS_ERR*	)&err);
				
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&startTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"task s", 			//任务名字
                 (OS_TASK_PTR )divTaskNbrS, 		//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )DivTaskSPrio,     	//任务优先级
                 (CPU_STK   * )&divTaskSStk[0],		//任务堆栈基地址
                 (CPU_STK_SIZE)DivTaskSStkSize / 10,//任务堆栈深度限位
                 (CPU_STK_SIZE)DivTaskSStkSize,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目，为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,//任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
				 
	OS_CRITICAL_EXIT();								//退出临界区	 
	OSStart(&err);      							//开启OS							
}

/*$PAGE*/
//信号量处理
void Semaphore_Handler (void)
{
}

//资源共享访问
//传参：资源区，任务字符串，延时时间(单位ms)
void ShareResources_Handler (u8 *resource, const void *task_str, u16 delay_time)
{
	memcpy(resource, task_str, sizeof(task_str));	//向共享资源区拷贝数据
	
	if (psaux_Switch == psaux_Enable)				//调试任务切换
	{
		__ShellHeadSymbol__; 
		if (SendDataCondition)
		{
			printf("Share Resource: %s\r\n", resource);//串口输出共享资源区数据
			usart1WaitForDataTransfer();		
		}
	}
	
	delay_ms(delay_time);
}

//内存块申请处理
void MemStack_ReqHandler (void)
{
	OS_ERR err;
	static u8 internal_memget_num;
	CPU_INT08U *internal_buf;
	
	if (KEY0_LTrigger && KEY1_LTrigger)				//双键同按
	{		
		internal_buf = OSMemGet((OS_MEM *)&osMemStorageArea, (OS_ERR *)&err);
		if (err == OS_ERR_NONE) 					//内存申请成功
		{
			__ShellHeadSymbol__; 
			if (SendDataCondition)
			{
				printf("Internal_buf Memory address is: %#x\r\n", (u32)(internal_buf));//%#x可以输出0x
				usart1WaitForDataTransfer();		
			}
			
			internal_memget_num++;
			
			__ShellHeadSymbol__; 
			if (SendDataCondition)
			{
				snprintf((char*)internal_buf, snprintfStackSpace, "INTERNAL_MEM Use %d times\r\n", internal_memget_num);
				usart1WaitForDataTransfer();		
			}
		}
		if (err == OS_ERR_MEM_NO_FREE_BLKS) 		//内存块不足
		{
			__ShellHeadSymbol__; U1SD("Internal Memory Not Enough\r\n");
		}
		
		//internal_buf不为空就释放内存
		if (internal_buf != NULL) 						
		{
			OSMemPut((OS_MEM *)&osMemStorageArea, (void *)internal_buf, (OS_ERR *)&err);//释放内存
			
			__ShellHeadSymbol__; 
			if (SendDataCondition)
			{
				printf("Internal_buf release Memory address is: %#x\r\n", (u32)(internal_buf));
				usart1WaitForDataTransfer();		
			}
		}
	}
	if (psaux_Switch == psaux_Enable)
	{
		if (SendDataCondition)
		{
			printf("Totol Remain: Max: %d | Free: %d \r\n", osMemStorageArea.NbrMax, osMemStorageArea.NbrFree);
			usart1WaitForDataTransfer();		
		}
	}
}

//osiii通用任务函数
void osiiiTaskFuction (	void (*ActualExeFunction)(void),//执行函数
						u8* srStr, 						//共享资源
						u8 srpTime, 					//资源延时
						u16 dlyTime)					//任务切换延时
{
	OS_ERR err;										
	
	while (True)										//任务循环
    {
		ActualExeFunction();							//任务函数
		ShareResources_Handler(share_resource, srStr, srpTime);//共享资源
		//延时任务切换
		if (dlyTime >= 1000u)
			OSTimeDlyHMSM(0, 0, dlyTime / 1000u, dlyTime - (dlyTime / 1000u) * 1000u, OS_OPT_TIME_PERIODIC, &err);
		else
			OSTimeDlyHMSM(0, 0, 0, dlyTime, OS_OPT_TIME_PERIODIC, &err);
    }
}

/*$PAGE*/
/*	
	任务栈区，只做基本分装，没有实体
	请务必准确估计任务栈的延时系数，不然RTOS跑起来效果会很差
*/
void divTaskNbr4 (void *p_arg)
{
	u8 tastStr[] = "Task 4 Running";
	p_arg = p_arg;	
	osiiiTaskFuction(prio4TaskBus, tastStr, SRpTime, 100u);
}

void divTaskNbr3 (void *p_arg)
{
	u8 tastStr[] = "Task 3 Running";
	p_arg = p_arg;	
	osiiiTaskFuction(prio3TaskBus, tastStr, SRpTime, 400u);
}

void divTaskNbr2 (void *p_arg)
{
	u8 tastStr[] = "Task 2 Running";
	p_arg = p_arg;	
	osiiiTaskFuction(prio2TaskBus, tastStr, SRpTime, 200u);
}

void divTaskNbr1 (void *p_arg)
{
	u8 tastStr[] = "Task 1 Running";
	p_arg = p_arg;	
	osiiiTaskFuction(prio1TaskBus, tastStr, SRpTime, 200u);
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
