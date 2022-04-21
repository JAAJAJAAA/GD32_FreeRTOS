#include "PID.h"

Pid_TypeDef Global_QPid; // Q轴电流PID
Pid_TypeDef Global_DPid; // D轴电流PID
Pid_TypeDef Global_Vpid; //速度环PID
Pid_TypeDef Global_Spid; //位置环PID

/***************************************************
 * 名称: PidCal
 * 描述: PID运算函数
 * 参数: pid-pid结构体;setObj-目标值;actObj-实际值
 * 返回: void
 ***************************************************/
void PidCal(Pid_TypeDef *pid, float setObj, float actObj)
{
	pid->SetObj = setObj;
	pid->ActualObj = actObj;

	pid->Error = pid->SetObj - pid->ActualObj;

	//变比例
	if (fabs(pid->Error) < pid->PError)
	{
		pid->POut = pid->Kp2 * pid->Error;
	}
	else
	{
		pid->POut = pid->Kp * pid->Error;
	}

	//积分分离
	if (fabs(pid->Error) < pid->IntegralIntervene)
	{
		pid->ErrorIntegral += pid->Error;
	}
	// 限制积分深度
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

	//限制PID输出大小
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
