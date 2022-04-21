#include "PID.h"

Pid_TypeDef Global_QPid; // Q�����PID
Pid_TypeDef Global_DPid; // D�����PID
Pid_TypeDef Global_Vpid; //�ٶȻ�PID
Pid_TypeDef Global_Spid; //λ�û�PID

/***************************************************
 * ����: PidCal
 * ����: PID���㺯��
 * ����: pid-pid�ṹ��;setObj-Ŀ��ֵ;actObj-ʵ��ֵ
 * ����: void
 ***************************************************/
void PidCal(Pid_TypeDef *pid, float setObj, float actObj)
{
	pid->SetObj = setObj;
	pid->ActualObj = actObj;

	pid->Error = pid->SetObj - pid->ActualObj;

	//�����
	if (fabs(pid->Error) < pid->PError)
	{
		pid->POut = pid->Kp2 * pid->Error;
	}
	else
	{
		pid->POut = pid->Kp * pid->Error;
	}

	//���ַ���
	if (fabs(pid->Error) < pid->IntegralIntervene)
	{
		pid->ErrorIntegral += pid->Error;
	}
	// ���ƻ������
	if (pid->ErrorIntegral > pid->ErrorIntegralMax)
	{
		pid->ErrorIntegral = pid->ErrorIntegralMax;
	}
	else if (pid->ErrorIntegral < pid->ErrorIntegralMin)
	{
		pid->ErrorIntegral = pid->ErrorIntegralMin;
	}

	pid->IOut = pid->Ki * (pid->ErrorIntegral);
	pid->DOut = pid->Kd * (pid->Error - pid->ErrorOld);
	pid->Out = pid->POut + pid->IOut + pid->DOut;

	//����PID�����С
	if (pid->Out > pid->OutMax)
	{
		pid->Out = pid->OutMax;
	}
	else if (pid->Out < pid->OutMin)
	{
		pid->Out = pid->OutMin;
	}

	pid->ErrorOld = pid->Error;
}
