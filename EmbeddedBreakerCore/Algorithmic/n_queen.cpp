#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	N皇后问题C++实现
	实现方法：使用vector和set的STL容器及迭代器完成全局回溯
	实验结果：
		皇后矩阵数	解法数	演算耗时
		7			40		0.9s
		8			92		7s
		9			352		78s
		10			742		15min
	请不要尝试计算太大数目的皇后计算，十分占用CPU资源
*/

using namespace std;						//标准命名空间

//回溯算法函数
void BackdateRecall (int num)				//传参：矩阵大小
{
	int count = 0;							//解法计数
	char result[10];						//存储结果
	//创建vector容器对象
	vector<int> border(num);				//棋盘		
	vector<int> afterAdd(num);				//叠加后
	vector<int> afterSub(num);				//叠减后

	//往border里装填
	for (int i = 0; i < num; ++i)
		border[i] = i + 1;					//每一行的上下左右都不会有其他的占位

	for (;;)								//for循环枚举所有情况
	{
		//迭代器回溯
		transform(border.begin(), border.end(), afterAdd.begin(),//叠加
		[](int i) -> int {
						static int index = 1; 
						i += index;			//叠加
						index++; 
						return i; 
					});
		transform(border.begin(), border.end(), afterSub.begin(),//叠减
		[](int i) -> int {
						static int index = 1;
						i -= index;			//叠减
						index++; 
						return i;
					});

		//创建set对象，获取前一个迭代值
	    set<int> afterAddSet(afterAdd.begin(), afterAdd.end());
		set<int> afterSubSet(afterSub.begin(), afterSub.end());

		//如果斜对角线上都没有元素
		if (afterAddSet.size() == num && afterSubSet.size() == num)
		{    
			++count;
			for (auto c : border) cout << c << ' ';//打印第N个棋子的放置位置
			cout << endl;					//最后换行
		}

		if (!next_permutation(border.begin(), border.end()))
		{
			cout << "Total Root: " << count << endl;
			sprintf_s(result, "%d", count);	//ANSI标准 变量转换成字符串
			MessageBoxA(NULL, result, "Total Root", MB_OK);//窗口显示结果
			break;							//枚举结束退出
		}
	}
}

//主函数
void nQueen_Handler (void)
{
	//logo
	cout << "[+]----N Queen Question Backdate Calculus----[+]" << endl;
	cout << "[+]------code by </MATRIX>@Neod Anderjon-----[+]" << endl;

	cout << "Enter Queen Number: ";
	cin >> queen_num;						//输入演算宽度

	BackdateRecall(queen_num);		
			
	//logo
	cout << "[+]------code by </MATRIX>@Neod Anderjon-----[+]" << endl;
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
