#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//DSP DF(数字信号处理，数字滤波器)

//卡尔曼一阶因子初始化
static void KF_1DerivFactor_Init (kf_1deriv_factor *kf)
{
	/*
		卡尔曼滤波参数的调整：
		p是初始化最优角度估计的协方差(初始化最优角度估计可设为0)
		q是预测值的协方差
		r是测量值的协方差
		q/(q+r)的值就是卡尔曼增益的收敛值，卡尔曼增益会向这个值收敛
		卡尔曼增益越小，说明预测值越可靠，最优化角度越接近预测值
		相反的，卡尔曼增益越大，说明测量值越可靠，最优化角度越接近测量值
		p/(q+r)反映收敛的快慢程度，该值设定越小，收敛越快，该值越大，收敛越慢
	*/
	kf -> x = 0.f;										//滤波变量(变量)
	kf -> y	= 0.f;										//滤波结果(变量)
	kf -> A = 1.f;
	kf -> H = 1.f;
	kf -> q = 0.003f;									//预测协方差
	kf -> r = 0.5f;										//测量协方差
	kf -> p = 0.f;										//最优角度估计协方差(变量)
	kf -> g = 0.f;										//卡尔曼增益(变量)
}	

/*
	一阶卡尔曼滤波
	传参：输入测量值，卡尔曼一阶结构体指针
*/
float Kalman_1DerivFilter (float mes, kf_1deriv_factor *kfstr)
{	
	float mat;
	static Bool_ClassType strHasInit = False;
	
	//只初始化一次结构体成员
	if (strHasInit == False)
	{
		strHasInit = True;
		KF_1DerivFactor_Init(kfstr);							
	}
							
	//以下只有P值得到继承，其他变量均在重新载入滤波函数时被刷新
	kfstr -> x = mes;									
	//状态预测
	kfstr -> x *= kfstr -> A;							
	//误差协方差预测，P值得到继承
	kfstr -> p = pow(kfstr -> A, 2) * kfstr -> p + kfstr -> q;
	//计算卡尔曼滤波增益，被除数不能为0，不然会返回NAN
	mat = ((kfstr -> p * pow(kfstr -> H, 2)) + kfstr -> r);
	if (!mat) kfstr -> g = kfstr -> p * kfstr -> H / mat;
	//状态估计校正
    kfstr -> x += kfstr -> g * (kfstr -> y - (kfstr -> H * kfstr -> x));
	//误差协方差估计校正
    kfstr -> p *= (1 - (kfstr -> g * kfstr -> H));
	//得出滤波结果
	kfstr -> y = kfstr -> x * kfstr -> H;

    return kfstr -> y;									//返回滤波结果
}

//卡尔曼二阶因子初始化
static void KF_2DerivFactor_Init (kf_2deriv_factor *kf)
{
	/*
		卡尔曼滤波参数的调整：
		p是初始化最优角度估计的协方差(初始化最优角度估计可设为0)
		q是预测值的协方差
		r是测量值的协方差
		q/(q+r)的值就是卡尔曼增益的收敛值，卡尔曼增益会向这个值收敛
		卡尔曼增益越小，说明预测值越可靠，最优化角度越接近预测值
		相反的，卡尔曼增益越大，说明测量值越可靠，最优化角度越接近测量值
		p/(q+r)反映收敛的快慢程度，该值设定越小，收敛越快，该值越大，收敛越慢
	*/
	//控制滤波变量为矩阵
	kf -> x[0]   	= 0.f;								//滤波变量
    kf -> x[1]    	= 0.f;
	kf -> y[0]		= 0.f;								//滤波结果
	kf -> y[1]		= 0.f;								
	kf -> A[0][0] 	= 1.f;
	kf -> A[0][1] 	= 0.1f;
	kf -> A[1][0] 	= 0.f;
	kf -> A[1][1] 	= 1.f;
	kf -> H[0] 		= 1.f;
	kf -> H[1] 		= 0.f;
	kf -> q[0]    	= 0.003f;							//预测协方差
    kf -> q[1]    	= 0.003f;
	kf -> r      	= 0.5f;  							//测量协方差
    kf -> p[0][0] 	= 0.01f;							//最优角度估计协方差
    kf -> p[0][1] 	= 0.01f;
	kf -> p[1][0] 	= 0.01f;
	kf -> p[1][1]	= 0.01f;
	kf -> g[0]		= 0.0f;								//卡尔曼增益
	kf -> g[1]		= 0.0f;
}

/*
	二阶卡尔曼滤波
	传参：输入测量值，卡尔曼二阶结构体指针
*/
__packed float* Kalman_2DerivFilter (float mes, kf_2deriv_factor *kfstr)
{
	float mat1, mat2, mat3;
	static Bool_ClassType strHasInit = False;
	
	//只初始化一次结构体成员
	if (strHasInit == False)
	{
		strHasInit = True;
		KF_2DerivFactor_Init(kfstr);						
	}

	//滤波变量赋值
	kfstr -> x[0] = mes;		
	kfstr -> x[1] = mes;
    //状态预测
    kfstr -> x[0] = kfstr -> A[0][0] * kfstr -> x[0] + kfstr -> A[0][1] * kfstr -> x[1];
    kfstr -> x[1] = kfstr -> A[1][0] * kfstr -> x[0] + kfstr -> A[1][1] * kfstr -> x[1];
    
	//误差协方差预测
	//p(n|n-1) = A^2*p(n-1|n-1) + q
    kfstr -> p[0][0] = kfstr -> A[0][0] * kfstr -> p[0][0] + kfstr -> A[0][1] * kfstr -> p[1][0] + kfstr -> q[0];
	kfstr -> p[0][1] = kfstr -> A[0][0] * kfstr -> p[0][1] + kfstr -> A[1][1] * kfstr -> p[1][1];
	kfstr -> p[1][0] = kfstr -> A[1][0] * kfstr -> p[0][0] + kfstr -> A[0][1] * kfstr -> p[1][0];
    kfstr -> p[1][1] = kfstr -> A[1][0] * kfstr -> p[0][1] + kfstr -> A[1][1] * kfstr -> p[1][1] + kfstr -> q[1];

	//计算卡尔曼滤波增益
    //g = p * H^T * [r + H * p * H^T]^(-1), H^T
    mat1 = kfstr -> p[0][0] * kfstr -> H[0] + kfstr -> p[0][1] * kfstr -> H[1];
	mat2 = kfstr -> p[1][0] * kfstr -> H[0] + kfstr -> p[1][1] * kfstr -> H[1];
	mat3 = kfstr -> r + kfstr -> H[0] * mat1 + kfstr -> H[1] * mat2;
	
	//被除数不能为0，不然会返回NAN
	if (!mat3)										
	{
		kfstr -> g[0] = mat1 / mat3;
		kfstr -> g[1] = mat2 / mat3;
	}
	
	//状态估计校正
    //x(n|n) = x(n|n-1) + g(n) * [mes - H(n)*x(n|n-1)]
    mat3 = kfstr -> H[0] * kfstr -> x[0] + kfstr -> H[1] * kfstr -> x[1];
    kfstr -> x[0] += kfstr -> g[0] * (kfstr -> y[0] - mat3); 
    kfstr -> x[1] += kfstr -> g[1] * (kfstr -> y[1] - mat3);

	//误差协方差估计校正
    //Update @p: p(n|n) = [I - g * H] * p(n|n-1)
    kfstr -> p[0][0] *= (1 - kfstr -> g[0] * kfstr -> H[0]);
	kfstr -> p[0][1] *= (1 - kfstr -> g[0] * kfstr -> H[1]);
	kfstr -> p[1][0] *= (1 - kfstr -> g[1] * kfstr -> H[0]);
    kfstr -> p[1][1] *= (1 - kfstr -> g[1] * kfstr -> H[1]);
	
	//滤波结果
	kfstr -> y[0] = kfstr -> x[0] * kfstr -> H[0];
	kfstr -> y[1] = kfstr -> x[1] * kfstr -> H[1];
	
    return kfstr -> y;										//返回二维数组指针
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
