#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//N皇后问题 CPU性能测试 结果一般比PC要快
/*
	计算结果：
	皇后数			解法数目
	7				40
	8				92
	9				352
	10				724
	11				2680
	12				14200
*/

#define QueenMatrixMax 		20u  									//定义结果数组的最大大小，可以扩充
int* a;    										

//问题演算处理
void nQueen_CalculusHandler (void) 
{
    int i = 1, k, flag, not_finish = 1, count = 0;					//正在处理的元素下标，表示前i-1个元素已符合要求，正在处理第i个元素
	
	u8 queenMatrixWidth = *(USART1_RX_BUF + NQueen_1Bit) * 10u 
		+ *(USART1_RX_BUF + NQueen_2Bit);//协议获得皇后数
	if (queenMatrixWidth > QueenMatrixMax)
	{
		queenMatrixWidth = 0u;
		
		__ShellHeadSymbol__; U1SD_E("Queen Matrix Width Error\r\n");//皇后数报错
		SERIALDATAERROR;
	}
	//若不传参则默认为8
	else if (!queenMatrixWidth)
		queenMatrixWidth = 8;
	
	a = (int*)mymalloc(sizeof(int) * (queenMatrixWidth + 1));		//N皇后问题的皇后所在的行列位置，从1幵始算起，所以加1

	//打印皇后数
	__ShellHeadSymbol__; 
	U1SD("Calculus Queen Number: %2d\r\n", queenMatrixWidth);		
	
	//演算
	*(a + 1) = 1;  													//为皇后的第一个元素赋初值
    while (not_finish && Return_Error_Type == Error_Clear) 			//无错状态下进行
	{ 					
		//处理尚未结束且还没处理到第Queens个元素		
        while (not_finish && (i <= queenMatrixWidth))					
		{ 
			//判断是否有多个皇后在同一行
            for (flag = 1, k = 1; flag && k < i; k++) 
			{
                if (*(a + k) == *(a + i)) 
					flag = 0;
			}
			
			//判断是否有多个皇后在同一对角线
            for (k = 1; flag && k < i; k++)  
			{
                if ((*(a + i) == *(a + k) - (k - i)) 
					|| (*(a + i) == *(a + k) + (k - i))) 
					flag = 0;
			}
					
			//若存在矛盾不满足要求，需要重新设置第i个元素
            if (!flag) 
			{ 
                if (*(a + i) == *(a + (i - 1))) 					//若a[i]的值已经经过一圈追上a[i-1]的值
				{ 
                    i--;  											//退回一步，重新试探处理前一个元素
                    if (i > 1 && *(a + i) == queenMatrixWidth) 
						*(a + i) = 1;								//当a[i]为Queens时将a[i]的值置1
                    else if (i == 1 && a[i] == queenMatrixWidth) 
						not_finish = 0;								//当第一位的值达到Queens时结束
                    else 
						(*(a + i))++;  								//将a[i]的值取下一个值
                } 
				else if (*(a + i) == queenMatrixWidth) 
					*(a + i) = 1;
                else 
					(*(a + i))++;   								//将a[i]的值取下一个值
            } 
			else if (++i <= queenMatrixWidth)
			{
                if (*(a + (i - 1)) == queenMatrixWidth) 
					*(a + i) = 1;									//若前一个元素的值为Queens则a[i]=1
				else 
					*(a + i) = *(a + (i - 1)) + 1;  				//否则元素的值为前一个元素的下一个值
			}
        }
		
        if (not_finish) 
		{
            ++count;
			//输出结果	
			U1SD((count - 1) % 3 ? "\r\n[%03d]----> " : "\r\n[%03d]----> ", count);		
			for (k = 1; k <= queenMatrixWidth; k++) 
			{				
				U1SD(" %02d ", *(a + k));							//打印序列
			}
			
            if (*(a + (queenMatrixWidth - 1)) < queenMatrixWidth) 
				(*(a + (queenMatrixWidth - 1)))++;					//修改倒数第二位的值
            else 
				*(a + (queenMatrixWidth - 1)) = 1;
			
            i = queenMatrixWidth - 1;    							//开始寻找下一个满足条件的解
        }
    }
	myfree(a);
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
