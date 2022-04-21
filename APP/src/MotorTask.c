/***************************************************
 * Description  :
 * LastEditTime : 2022-04-21 10:30:41
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\MotorTask.c
 ***************************************************/
#include "MotorTask.h"

// 电机控制数据
Moto_Cal_datatypedef Global_Motor_Cal;
// 电机控制任务句柄
TaskHandle_t Global_MotorTaskHandle;
// 电流采集信号量句柄
SemaphoreHandle_t Global_AdcHandle;
// FOC计算数据
extern FOC_typedef Global_Foc;
// 电机状态
MotorStatus_TypeDef Global_MotorStatus;
// 3路PWM
u16 Global_Pwm[3];
// adc信号量
SemaphoreHandle_t osSemaphoreDef_Adc_Sem;
extern uint16_t currentdata[2];
extern MotorCaliParam_TypeDef Global_MotorCaliParam;
/***************************************************
 名字:	Moto_Task_init
 参数:	void
 功能:	电机任务初始化
 返回:	void
 ***************************************************/
void Motor_Task_Init(void)
{

	// Global_AdcHandle = osSemaphoreCreate(osSemaphore(Adc_Sem), 1);

	Global_AdcHandle = xSemaphoreCreateBinary();

	if (Global_AdcHandle != NULL)
	{
		xSemaphoreGive(Global_AdcHandle);
	}

	xTaskCreate((TaskFunction_t)Motor_Task,				  //任务函数
				(const char *)"Motor_Task",				  //任务名称
				(uint16_t)configMINIMAL_STACK_SIZE,		  //任务堆栈大小
				(void *)NULL,							  //传递给任务函数的参数
				(UBaseType_t)3,							  //任务优先级
				(TaskHandle_t *)&Global_MotorTaskHandle); //任务句柄
	printf(" The Motor Task initialization succeeded \r\n");

	Global_MotorStatus = NORMAL;

	Global_DPid.Kp = 0.0000f;
	Global_DPid.Ki = 0.00000f;
	Global_DPid.Kd = 0.0000f;

	Global_DPid.PError = 0.01f;

	Global_DPid.ErrorIntegralMax = 100.0f;
	Global_DPid.ErrorIntegralMin = -100.0f;

	Global_DPid.OutMax = 0.01f;
	Global_DPid.OutMin = -0.01f;

	Global_QPid.Kp = 0.05f;	  // 0.2f;         //0.05
	Global_QPid.Kp2 = 0.1f;	  // 0.25f          // 0.1
	Global_QPid.Ki = 0.0001f; // 0.00003f;
	Global_QPid.Kd = 0.0000f;

	Global_QPid.PError = 0.01f;

	Global_QPid.IntegralIntervene = 5.0f;

	Global_QPid.ErrorIntegralMax = 50.0f;
	Global_QPid.ErrorIntegralMin = -50.0f;

	Global_QPid.OutMax = 1.0f;	// 0.6f
	Global_QPid.OutMin = -1.0f; // 0.6f
}

/***************************************************
 * 名称: Motor_Task
 * 描述: 电机运行任务
 * 参数: void *pvParameters
 * 返回: void
 ***************************************************/
void Motor_Task(void *pvParameters)
{
	while (1)
	{
		// vTaskDelay(50);
		// printf("-");
		if (SemaphoreWait(Global_AdcHandle, WAIT_FOREVER) == osOK) //等待电流采集的信号
		{
			//读编码器值
			Global_Motor_Cal.EncoderValUse = GetEncoder();
			PWM_Cal();
			PWM_Set(Global_Pwm);
		}
	}
}

/***************************************************
 * 名称: SemaphoreWait
 * 描述: 等待电流信号采集函数
 * 参数: semaphore_id-信号量句柄,millisec-等待时间
 * 返回: uint32_t
 ***************************************************/
uint32_t SemaphoreWait(SemaphoreHandle_t semaphore_id, u32 millisec)
{
	TickType_t ticks;
	portBASE_TYPE taskWoken = pdFALSE;

	if (semaphore_id == NULL)
	{
		return osErrorParameter;
		// printf(" A mandatory parameter was missing or specified an incorrect object\r\n");
	}

	ticks = 0;
	if (millisec == WAIT_FOREVER)
	{
		ticks = portMAX_DELAY;
	}
	else if (millisec != 0)
	{
		ticks = millisec / portTICK_PERIOD_MS;
		if (ticks == 0)
		{
			ticks = 1;
		}
	}
	if (__get_IPSR() != 0)
	{
		if (xSemaphoreTakeFromISR(semaphore_id, &taskWoken) != pdTRUE)
		{
			return osErrorOS;
		}
		portEND_SWITCHING_ISR(taskWoken);
	}
	else if (xSemaphoreTake(semaphore_id, ticks) != pdTRUE)
	{
		return osErrorOS;
	}

	return osOK;
}

/***************************************************
 * 名称: GetEncoderVal
 * 描述: 获取编码器值,寄存器输出值定义上是一个有符号数，直接处理为无符号数
 * 参数: void
 * 返回: u16
 ***************************************************/
uint16_t GetEncoder(void)
{
	uint16_t EncoderRaw = 0;
	//    if(Global_MotoRunDir == CW){
	EncoderRaw = ENCODER_LINE - MT6815_READ_NEW_DATA();
	//    }else{
	//    	EncoderRaw =  MT6815_READ_NEW_DATA();
	//    }
	//获取编码器值
	return EncoderRaw;
}

/***************************************************
 * 名称: ClearDR
 * 描述: 清除数据
 * 参数: void
 * 返回: void
 ***************************************************/
uint16_t ClearDR(void)
{
	// return Global_hspi2.Instance->DR;
	return 0;
}

/***************************************************
 * 名称: CsON
 * 描述: 使能片选
 * 参数: void
 * 返回: void
 ***************************************************/
void CsON(void)
{
	// HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
}

/***************************************************
 * 名称: CsOFF
 * 描述: 禁止片选
 * 参数: void
 * 返回: void
 ***************************************************/
void CsOFF(void)
{
	// HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
}

/***************************************************
 * 名称: ElecDegCal
 * 描述: 计算电机电角度
 * 参数: uint16_t encoderVal 编码器值
 * 返回: void
 ***************************************************/
void ElecDegCal(uint16_t encoderVal)
{
	float EelecAngleTmp;
	// 电角度值 = （编码器读取值 - 零相位校准值）*电机极对数*2π/编码器精度
	EelecAngleTmp = ((float)encoderVal - Global_MotorCaliParam.PhaseParam) * MOTOR_PAIR_F * 2.0f * PI / ENCODER_LINE_F;
	//电角度值限制范围在0-2π之内
	while (EelecAngleTmp > PI)
	{
		EelecAngleTmp -= 2.0f * PI;
	}
	//电角度值限制范围在0-2π之内
	while (EelecAngleTmp < -PI)
	{
		EelecAngleTmp += 2.0f * PI;
	}

	Global_Motor_Cal.ElecDeg = EelecAngleTmp;
}

/***************************************************
 * 名称: CurrentCal
 * 描述: 根据基尔霍夫定律计算ABC三相电流     Ia+Ib+Ic =0
 * 参数: float deg 电角度值
 * 		uint16_t*CurrentData  采集的电流原始值
 * 返回: void
 ***************************************************/
void CurrentCal(float deg, uint16_t *CurrentData)
{
	Global_Foc.Ia = (((int32_t)CurrentData[0] - Global_MotorCaliParam.CurrentOffset[0]) * 3.3f / 4096.0f - 1.65f) / 0.6f;
	Global_Foc.Ib = (((int32_t)CurrentData[1] - Global_MotorCaliParam.CurrentOffset[1]) * 3.3f / 4096.0f - 1.65f) / 0.6f;
	Global_Foc.Ic = -Global_Foc.Ia - Global_Foc.Ib;
	//克拉克变换,三相变两项
	Clark_f32(Global_Foc.Ia, Global_Foc.Ib, Global_Foc.Ic, &Global_Foc.Ialpha, &Global_Foc.Ibeta);
	//帕克变换,动坐标变为静坐标
	Park_f32(Global_Foc.Ialpha, Global_Foc.Ibeta, &Global_Foc.Id, &Global_Foc.Iq, arm_sin_f32(deg),
			 arm_cos_f32(deg));
}
/***************************************************
 * 名称: PWM_Cal
 * 描述:
 * 参数: void
 * 返回: void
 ***************************************************/
void PWM_Cal(void)
{
	float radio;
	switch (Global_MotorStatus)
	{
		//正常状态
	case NORMAL:
		//获取用户设置的力矩值，用来限制最大力矩输出
		radio = (float)Get_Store_UserParam().Torque / 100.0f;
		//计算电机电角度
		ElecDegCal(Global_Motor_Cal.EncoderValUse);
		//计算ABC三相电流
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);

		Global_QPid.SetObj = Global_Vpid.Out;
		Global_QPid.ActualObj = Global_Foc.Iq;

		Global_DPid.SetObj = 0.0f;
		Global_DPid.ActualObj = Global_Foc.Id;

		PidCal(&Global_QPid, Global_QPid.SetObj, Global_QPid.ActualObj);
		PidCal(&Global_DPid, Global_DPid.SetObj, Global_DPid.ActualObj);

		Global_Foc.Uq = Global_QPid.Out;
		Global_Foc.Ud = Global_DPid.Out;

		//根据用户设置限制力矩输出
		if (Global_Foc.Uq > Global_QPid.OutMax * radio)
		{
			Global_Foc.Uq = Global_QPid.OutMax * radio;
		}
		else if (Global_Foc.Uq < Global_QPid.OutMin * radio)
		{
			Global_Foc.Uq = Global_QPid.OutMin * radio;
		}

		// Park逆变换
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta,
					arm_sin_f32(Global_Motor_Cal.ElecDeg), arm_cos_f32(Global_Motor_Cal.ElecDeg));
		//修改过后的克拉克逆变换
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);
		// SVPWM 计算 PWM值输出
		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		break;
		//待机状态
	case STANDBY:
		ElecDegCal(Global_Motor_Cal.EncoderValUse);
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);
		Global_Pwm[0] = 0;
		Global_Pwm[1] = 0;
		Global_Pwm[2] = 0;
		break;
		//锁定状态 用于校准电角度0位
	case LOCK:
		CurrentCal(0, currentdata);
		Global_Foc.Uq = 0;
		Global_Foc.Ud = MOTOCAL_KEEP_VOLTAGE;

		// Park逆变换
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta, arm_sin_f32(0), arm_cos_f32(0));
		//修改过后的克拉克逆变换
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);

		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		break;
		//强制旋转 （用于调试Park变换和克拉克逆变换以及SVPWM)
	case FORCE_TURN:
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);
		Global_Foc.Uq = 0.0f;
		Global_Foc.Ud = MOTOCAL_KEEP_VOLTAGE;

		// Park逆变换
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta, arm_sin_f32(Global_Motor_Cal.ElecDeg), arm_cos_f32(Global_Motor_Cal.ElecDeg));
		//修改过后的克拉克逆变换
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);

		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		Global_Motor_Cal.ElecDeg += MOTOR_DEBUG_ROTATE_FORCE_RATE;
		break;
		//自由旋转（用于调试电流环）
	case FREE_TRUN:
		ElecDegCal(Global_Motor_Cal.EncoderValUse);
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);
		Global_QPid.SetObj = MOTOCAL_FREE_TURN_VOLTAGE;
		Global_QPid.ActualObj = Global_Foc.Iq;

		Global_DPid.SetObj = 0.00f;
		Global_DPid.ActualObj = Global_Foc.Id;

		PidCal(&Global_QPid, Global_QPid.SetObj, Global_QPid.ActualObj);
		PidCal(&Global_DPid, Global_DPid.SetObj, Global_DPid.ActualObj);

		Global_Foc.Uq = Global_QPid.Out;
		Global_Foc.Ud = Global_DPid.Out;

		// Park逆变换
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta, arm_sin_f32(Global_Motor_Cal.ElecDeg), arm_cos_f32(Global_Motor_Cal.ElecDeg));
		//修改过后的克拉克逆变换
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);
		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		break;
	default:
		break;
	}
}

/***************************************************
 * 名称: Svpwm
 * 描述: Svpwm计算
 * 参数: u1,u2,u3-输出电压
 * 返回: void
 ***************************************************/
void Svpwm(float u1, float u2, float u3, FlagStatus Rev, uint16_t *PWMX)
{

	uint8_t flagA, flagB, flagC, flagN;
	uint32_t TX, TY, Tx, Ty, Taon, Tbon, Tcon;
	uint16_t PWMX_TMP[3] = {0, 0, 0};
	uint16_t tmp = 0;
	float tmpf1;

	//判断扇面
	flagA = (u1 > 0) ? 1 : 0;
	flagB = (u2 > 0) ? 1 : 0;
	flagC = (u3 > 0) ? 1 : 0;

	flagN = flagA + (flagB << 1) + (flagC << 2);

	switch (flagN)
	{
	case 3:					   //扇区1
		TX = u2 * MOTO_COUNT2; //先发生的矢量
		TY = u1 * MOTO_COUNT2; //后发生的矢量

		if ((TX + TY) > MOTO_COUNT)
		{ //矫正过调制
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Taon; // T0(011，下桥臂导通的时间占比)
		PWMX_TMP[1] = Tbon; // T0+T4（001下桥臂导通的时间占比）
		PWMX_TMP[2] = Tcon; // T0+T4+T6（000 下桥臂导通的时间占比）
		break;
	case 1:						//扇区2(注意此1非扇区1)
		TX = -u2 * MOTO_COUNT2; //先发生的矢量
		TY = -u3 * MOTO_COUNT2; //后发生的矢量

		if ((TX + TY) > MOTO_COUNT)
		{ //矫正过调制
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Tbon;
		PWMX_TMP[1] = Taon;
		PWMX_TMP[2] = Tcon;
		break;
	case 5: //扇区3
		TX = u1 * MOTO_COUNT2;
		TY = u3 * MOTO_COUNT2;

		if ((TX + TY) > MOTO_COUNT)
		{
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Tcon;
		PWMX_TMP[1] = Taon;
		PWMX_TMP[2] = Tbon;
		break;
	case 4: //扇区4
		TX = -u1 * MOTO_COUNT2;
		TY = -u2 * MOTO_COUNT2;

		if ((TX + TY) > MOTO_COUNT)
		{
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Tcon;
		PWMX_TMP[1] = Tbon;
		PWMX_TMP[2] = Taon;
		break;
	case 6: //扇区5
		TX = u3 * MOTO_COUNT2;
		TY = u2 * MOTO_COUNT2;

		if ((TX + TY) > MOTO_COUNT)
		{
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Tbon;
		PWMX_TMP[1] = Tcon;
		PWMX_TMP[2] = Taon;
		break;
	case 2: //扇区6
		TX = -u3 * MOTO_COUNT2;
		TY = -u1 * MOTO_COUNT2;

		if ((TX + TY) > MOTO_COUNT)
		{
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Taon;
		PWMX_TMP[1] = Tcon;
		PWMX_TMP[2] = Tbon;
		break;
	}

	//反向
	if (!Rev)
	{
		tmp = PWMX_TMP[0];
		PWMX_TMP[0] = PWMX_TMP[1];
		PWMX_TMP[1] = tmp;
	}

	PWMX[0] = PWMX_TMP[0];
	PWMX[1] = PWMX_TMP[1];
	PWMX[2] = PWMX_TMP[2];
}

/***************************************************
 * 名称: SetPwmVal
 * 描述: 设置电机PMM输出
 * 参数: val
 * 返回: void
 ***************************************************/
void PWM_Set(uint16_t *val)
{
	taskDISABLE_INTERRUPTS();
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, MOTO_COUNT - val[0]);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, MOTO_COUNT - val[1]);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, MOTO_COUNT - val[2]);

	taskENABLE_INTERRUPTS();
}
