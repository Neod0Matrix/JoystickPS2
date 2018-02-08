#include "stdafx.h"								
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//动态内存管理
//避免使用malloc()和free()，在RTOS中很容易导致内存碎片

//内存池(4字节对齐)
__align(sizeof(int)) u8 membase[MEM_MAX_SIZE];	//SRAM内存池
//内存管理表
u16 memmapbase[MEM_ALLOC_TABLE_SIZE];			//SRAM内存池MAP
//内存管理参数	   
const u32 memtblsize = MEM_ALLOC_TABLE_SIZE;	//内存表大小
const u32 memblksize = MEM_BLOCK_SIZE;			//内存分块大小
const u32 memsize = MEM_MAX_SIZE;				//内存总大小

u8 MemUsagePercent;	

//内存管理控制器
_m_mallco_dev mallco_dev =
{
	Mem_Manager_Init,							//内存初始化
	mem_perused,								//内存使用率
	membase,									//内存池
	memmapbase,									//内存管理状态表
	0,  										//内存管理未就绪
};

/*
	复制内存
	*des:目的地址
	*src:源地址
	n:需要复制的内存长度(字节为单位)
*/
void mymemcpy (void *des, void *src, u32 n)  
{  
    u8 *xdes = des;
	u8 *xsrc = src; 
	
    while (n--) 
		*xdes++ = *xsrc++;  
}  

/*
	设置内存
	*s:内存首地址
	c :要设置的值
	count:需要设置的内存大小(字节为单位)
*/
void mymemset (void *s, u8 c, u32 count)  
{  
    u8 *xs = s;  
	
    while (count--) 
		*xs++ = c;  
}	   

//内存管理初始化  
void Mem_Manager_Init (void)  
{  
    mymemset(mallco_dev.memmap, 0, memtblsize * 2);//内存状态表数据清零  
	mymemset(mallco_dev.membase, 0, memsize);	//内存池所有数据清零  
	mallco_dev.memrdy = 1;						//内存管理初始化OK  
}  

//获取内存使用率
//返回值:使用率(0~100)
u8 mem_perused (void)  
{  
    u32 i, used = 0;  
	u8 memUsage;
	
    for (i = 0; i < memtblsize; i++)  
        if (mallco_dev.memmap[i]) 
			used++; 
		
	memUsage = (used * 100) / (memtblsize);  
	MemUsagePercent = memUsage;					//全局传参
	
    return memUsage;
}  

/*
	内存分配(内部调用)
	memx:所属内存块
	size:要分配的内存大小(字节)
	返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
*/
u32 mem_malloc (u32 size)  
{  
    signed long offset = 0;  
    u16 nmemb;									//需要的内存块数  
	u16 cmemb = 0;								//连续空内存块数
    u32 i;  
	
    if (!mallco_dev.memrdy) mallco_dev.init();	//未初始化,先执行初始化 
    if (size == 0) return 0XFFFFFFFF;			//不需要分配
	
    nmemb = size / memblksize;  				//获取需要分配的连续内存块数
    if (size % memblksize) nmemb++;  
	
    for (offset = memtblsize - 1; offset >= 0; offset--)//搜索整个内存控制区  
    {     
		if (!mallco_dev.memmap[offset]) cmemb++;//连续空内存块数增加
		else cmemb = 0;							//连续内存块清零
		if (cmemb == nmemb)						//找到了连续nmemb个空内存块
		{
            for (i = 0; i < nmemb; i++)  		//标注内存块非空 
            {  
                mallco_dev.memmap[offset + i] = nmemb;  
            }  
            return (offset * memblksize);		//返回偏移地址  
		}
    }  
	
    return 0XFFFFFFFF;							//未找到符合分配条件的内存块  
}  

/*
	释放内存(内部调用) 
	offset:内存地址偏移
	返回值:0,释放成功;1,释放失败;  
*/
u8 mem_free (u32 offset)  
{  
    int i, index, nmemb;  
	
    if (!mallco_dev.memrdy)						//未初始化,先执行初始化
	{
		mallco_dev.init();    
        return 1;								//未初始化  
    }  
    if (offset < memsize)						//偏移在内存池内
    {  
        index = offset / memblksize;			//偏移所在内存块号码  
        nmemb = mallco_dev.memmap[index];		//内存块数量
        for (i = 0; i < nmemb; i++)  			//内存块清零
        {  
            mallco_dev.memmap[index + i] = 0;  
        }
		
        return 0;  
    }
	else 
		return 2;								//偏移超区了  
}  

//释放内存(外部调用) ptr:内存首地址 
void myfree (void *ptr)							//为方便适应任何数据类型的数组，使用void  
{   
    if (ptr == NULL) return;					//地址为0，已经释放
	
    mem_free((u32)ptr - (u32)mallco_dev.membase);//释放内存    
	ptr = NULL;									//指向空地址
}  

/*
	分配内存(外部调用)
	size:内存大小(字节)
	返回值:分配到的内存首地址
*/
void *mymalloc (u32 size)  
{  					      				   
    if (mem_malloc(size) == 0XFFFFFFFF) 
		return NULL;  
    else 
		return (void*)((u32)mallco_dev.membase + mem_malloc(size));  
}  

/*
	重新分配内存(外部调用)
	*ptr:旧内存首地址
	size:要分配的内存大小(字节)
	返回值:新分配到的内存首地址
*/
void *myrealloc (void *ptr, u32 size)  
{  
    if (mem_malloc(size) == 0XFFFFFFFF) 
		return NULL;     
    else  
    {  			
		//拷贝旧内存内容到新内存   
	    mymemcpy((void*)((u32)mallco_dev.membase + mem_malloc(size)), ptr, size);
        myfree(ptr);  							//释放旧内存
		
        return (void*)((u32)mallco_dev.membase + mem_malloc(size));//返回新内存首地址
    }  
}

/*
	内存申请处理函数 
	在函数内建过程中只用一次，避免重复申请，必要时使用Bool_ClassType的标志进行操作
	传参：数组，数组大小(使用Get_Array_Size带参宏的方法获取)
	在数组进栈时申请，在数组退栈时使用myfree释放
*/
void Mem_RequestHandler (float *array, u16 array_width)
{ 
 	float *p = 0u, *tp = 0u;   					//内存指针
	u16 byte_space = array_width * sizeof(float);//计算需要的内存大小
	
	p = mymalloc(byte_space);					//申请相应数组的字节空间
	
	__ShellHeadSymbol__; 
	if (SendDataCondition)
	{
		printf("Array Size: %d | Request Malloc Space: %dbytes\r\n", array_width, byte_space);
		usart1WaitForDataTransfer();
	}
	
	if (p != NULL) 
		p = array;								//向p写入一些内容
	else 
	{
		__ShellHeadSymbol__; U1SD("Memory Stack Over Flow\r\n");//申请失败
	}
	
	if (tp != p)								//内容变更
	{
		tp = p;									//重新写入
		
		__ShellHeadSymbol__; 
		if (SendDataCondition)
		{
			printf("Array Request Space Address: %#8x\r\n", (u32)tp);
			usart1WaitForDataTransfer();
		}
	}
	
	myfree(p);									//释放内存
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
