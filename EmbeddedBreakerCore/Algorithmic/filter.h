#pragma once
#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
//DSP DF(数字信号处理，数字滤波器)	

/*
	一阶互补滤波 first order complementary filter
	am: 	测量输入加速度
	gm: 	测量输入陀螺仪
	k:  	权值系数
	dt:		一阶时间(由程序执行时间确认，一般取0.005左右)
	ao:		加速度滤波输出
*/
#ifndef FirstOrderComplementaryFilter
#define FirstOrderComplementaryFilter(am, gm, k, dt, ao) {ao = am * k + (1 - k) * (ao + gm * dt);}   
#endif

//一维卡尔曼滤波结构体
typedef __packed struct
{
	//变量
	float 			x;  						//输入测量值
	float 			y;							//滤波结果
	float 			p;  						//estimated error convariance 
    float 			g;
	//常量
    float 			A;  						//x(n)=A*x(n-1)+u(n),u(n)~N(0,q)
    float 			H;  						//z(n)=H*x(n)+w(n),w(n)~N(0,r) 
    float 			q;  						//process(predict) noise convariance 
    float 			r; 	 						//measure noise convariance
} kf_1deriv_factor;								

//二维卡尔曼滤波结构体
typedef __packed struct 
{
	//变量
    float 			x[2];    					//state: [0]-angle [1]-diffrence of angle, 2x1 
	float 			y[2];						//滤波结果
	float 			p[2][2];  					//estimated error convariance,2x2 [p0 p1; p2 p3] 
    float 			g[2];  						//2x1 
	//常量
    float 			A[2][2];  					//X(n)=A*X(n-1)+U(n),U(n)~N(0,q), 2x2 
    float 			H[2];     					//Z(n)=H*X(n)+W(n),W(n)~N(0,r), 1x2  
    float 			q[2];     					//process(predict) noise convariance,2x1 [q0,0; 0,q1] 
    float 			r;        					//measure noise convariance 
} kf_2deriv_factor;
	
//结构体成员初始化
extern void KF_1DerivFactor_Init (kf_1deriv_factor *kf);	
extern void KF_2DerivFactor_Init (kf_2deriv_factor *kf);

//滤波处理
extern float Kalman_1DerivFilter (float mes, kf_1deriv_factor *kfstr);
extern __packed float* Kalman_2DerivFilter (float mes, kf_2deriv_factor *kfstr);

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
