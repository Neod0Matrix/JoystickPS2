#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//N皇后芯片性能测试

#ifdef use_cpp_algo					//宏预编译C++算法
//C++版本
//#ifdef  __cplusplus
//extern "C" {
//#endif

//#include <iostream>
//#include <algorithm>
//#include <vector>
//#include <set>
//#include <stdlib.h>
//
//void BackdateRecall (int num);
//void nQueen_Handler (void);
//
//#ifdef  __cplusplus
//}
//#endif
#else
//C版本
void nQueen_CalculusHandler (void);
#endif

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
