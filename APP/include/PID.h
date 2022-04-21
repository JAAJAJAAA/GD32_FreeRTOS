/***************************************************
 * Description  : PID头文件
 * LastEditTime : 2022-04-13 16:55:49
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\include\PID.h
 ***************************************************/

#ifndef _PID_H_
#define _PID_H_

#include "UserConfig.h"
#include "typedef.h"

typedef struct
{
	float Kp;
	float Kp2;
	float Ki;
	float Kd;

	float SetObj;
	float ActualObj;

	float Error;		 //本次误差
	float ErrorOld;		 //上一次误差
	float ErrorIntegral; //误差积分

	float PError;			 //误差小于这个值更改比例
	float IntegralIntervene; //误差小于这个值引入积分

	float ErrorIntegralMax; //误差积分最大值
	float ErrorIntegralMin; //误差积分最小值

	//
	float POut;
	float IOut;
	float DOut;

	float Out;
	float OutLast;

	float OutMax;
	float OutMin;
} Pid_TypeDef;
void PidCal(Pid_TypeDef *pid, float setObj, float actObj);
extern Pid_TypeDef Global_QPid;
extern Pid_TypeDef Global_DPid;
extern Pid_TypeDef Global_Vpid;
extern Pid_TypeDef Global_Spid;
#endif
