/***************************************************
 * Description  : PIDͷ�ļ�
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

	float Error;		 //�������
	float ErrorOld;		 //��һ�����
	float ErrorIntegral; //������

	float PError;			 //���С�����ֵ���ı���
	float IntegralIntervene; //���С�����ֵ�������

	float ErrorIntegralMax; //���������ֵ
	float ErrorIntegralMin; //��������Сֵ

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
