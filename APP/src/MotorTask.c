/***************************************************
 * Description  :
 * LastEditTime : 2022-04-21 10:30:41
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\MotorTask.c
 ***************************************************/
#include "MotorTask.h"

// �����������
Moto_Cal_datatypedef Global_Motor_Cal;
// �������������
TaskHandle_t Global_MotorTaskHandle;
// �����ɼ��ź������
SemaphoreHandle_t Global_AdcHandle;
// FOC��������
extern FOC_typedef Global_Foc;
// ���״̬
MotorStatus_TypeDef Global_MotorStatus;
// 3·PWM
u16 Global_Pwm[3];
// adc�ź���
SemaphoreHandle_t osSemaphoreDef_Adc_Sem;
extern uint16_t currentdata[2];
extern MotorCaliParam_TypeDef Global_MotorCaliParam;
/***************************************************
 ����:	Moto_Task_init
 ����:	void
 ����:	��������ʼ��
 ����:	void
 ***************************************************/
void Motor_Task_Init(void)
{

	// Global_AdcHandle = osSemaphoreCreate(osSemaphore(Adc_Sem), 1);

	Global_AdcHandle = xSemaphoreCreateBinary();

	if (Global_AdcHandle != NULL)
	{
		xSemaphoreGive(Global_AdcHandle);
	}

	xTaskCreate((TaskFunction_t)Motor_Task,				  //������
				(const char *)"Motor_Task",				  //��������
				(uint16_t)configMINIMAL_STACK_SIZE,		  //�����ջ��С
				(void *)NULL,							  //���ݸ��������Ĳ���
				(UBaseType_t)3,							  //�������ȼ�
				(TaskHandle_t *)&Global_MotorTaskHandle); //������
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
 * ����: Motor_Task
 * ����: �����������
 * ����: void *pvParameters
 * ����: void
 ***************************************************/
void Motor_Task(void *pvParameters)
{
	while (1)
	{
		// vTaskDelay(50);
		// printf("-");
		if (SemaphoreWait(Global_AdcHandle, WAIT_FOREVER) == osOK) //�ȴ������ɼ����ź�
		{
			//��������ֵ
			Global_Motor_Cal.EncoderValUse = GetEncoder();
			PWM_Cal();
			PWM_Set(Global_Pwm);
		}
	}
}

/***************************************************
 * ����: SemaphoreWait
 * ����: �ȴ������źŲɼ�����
 * ����: semaphore_id-�ź������,millisec-�ȴ�ʱ��
 * ����: uint32_t
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
 * ����: GetEncoderVal
 * ����: ��ȡ������ֵ,�Ĵ������ֵ��������һ���з�������ֱ�Ӵ���Ϊ�޷�����
 * ����: void
 * ����: u16
 ***************************************************/
uint16_t GetEncoder(void)
{
	uint16_t EncoderRaw = 0;
	//    if(Global_MotoRunDir == CW){
	EncoderRaw = ENCODER_LINE - MT6815_READ_NEW_DATA();
	//    }else{
	//    	EncoderRaw =  MT6815_READ_NEW_DATA();
	//    }
	//��ȡ������ֵ
	return EncoderRaw;
}

/***************************************************
 * ����: ClearDR
 * ����: �������
 * ����: void
 * ����: void
 ***************************************************/
uint16_t ClearDR(void)
{
	// return Global_hspi2.Instance->DR;
	return 0;
}

/***************************************************
 * ����: CsON
 * ����: ʹ��Ƭѡ
 * ����: void
 * ����: void
 ***************************************************/
void CsON(void)
{
	// HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
}

/***************************************************
 * ����: CsOFF
 * ����: ��ֹƬѡ
 * ����: void
 * ����: void
 ***************************************************/
void CsOFF(void)
{
	// HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
}

/***************************************************
 * ����: ElecDegCal
 * ����: ��������Ƕ�
 * ����: uint16_t encoderVal ������ֵ
 * ����: void
 ***************************************************/
void ElecDegCal(uint16_t encoderVal)
{
	float EelecAngleTmp;
	// ��Ƕ�ֵ = ����������ȡֵ - ����λУ׼ֵ��*���������*2��/����������
	EelecAngleTmp = ((float)encoderVal - Global_MotorCaliParam.PhaseParam) * MOTOR_PAIR_F * 2.0f * PI / ENCODER_LINE_F;
	//��Ƕ�ֵ���Ʒ�Χ��0-2��֮��
	while (EelecAngleTmp > PI)
	{
		EelecAngleTmp -= 2.0f * PI;
	}
	//��Ƕ�ֵ���Ʒ�Χ��0-2��֮��
	while (EelecAngleTmp < -PI)
	{
		EelecAngleTmp += 2.0f * PI;
	}

	Global_Motor_Cal.ElecDeg = EelecAngleTmp;
}

/***************************************************
 * ����: CurrentCal
 * ����: ���ݻ��������ɼ���ABC�������     Ia+Ib+Ic =0
 * ����: float deg ��Ƕ�ֵ
 * 		uint16_t*CurrentData  �ɼ��ĵ���ԭʼֵ
 * ����: void
 ***************************************************/
void CurrentCal(float deg, uint16_t *CurrentData)
{
	Global_Foc.Ia = (((int32_t)CurrentData[0] - Global_MotorCaliParam.CurrentOffset[0]) * 3.3f / 4096.0f - 1.65f) / 0.6f;
	Global_Foc.Ib = (((int32_t)CurrentData[1] - Global_MotorCaliParam.CurrentOffset[1]) * 3.3f / 4096.0f - 1.65f) / 0.6f;
	Global_Foc.Ic = -Global_Foc.Ia - Global_Foc.Ib;
	//�����˱任,���������
	Clark_f32(Global_Foc.Ia, Global_Foc.Ib, Global_Foc.Ic, &Global_Foc.Ialpha, &Global_Foc.Ibeta);
	//���˱任,�������Ϊ������
	Park_f32(Global_Foc.Ialpha, Global_Foc.Ibeta, &Global_Foc.Id, &Global_Foc.Iq, arm_sin_f32(deg),
			 arm_cos_f32(deg));
}
/***************************************************
 * ����: PWM_Cal
 * ����:
 * ����: void
 * ����: void
 ***************************************************/
void PWM_Cal(void)
{
	float radio;
	switch (Global_MotorStatus)
	{
		//����״̬
	case NORMAL:
		//��ȡ�û����õ�����ֵ��������������������
		radio = (float)Get_Store_UserParam().Torque / 100.0f;
		//��������Ƕ�
		ElecDegCal(Global_Motor_Cal.EncoderValUse);
		//����ABC�������
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);

		Global_QPid.SetObj = Global_Vpid.Out;
		Global_QPid.ActualObj = Global_Foc.Iq;

		Global_DPid.SetObj = 0.0f;
		Global_DPid.ActualObj = Global_Foc.Id;

		PidCal(&Global_QPid, Global_QPid.SetObj, Global_QPid.ActualObj);
		PidCal(&Global_DPid, Global_DPid.SetObj, Global_DPid.ActualObj);

		Global_Foc.Uq = Global_QPid.Out;
		Global_Foc.Ud = Global_DPid.Out;

		//�����û����������������
		if (Global_Foc.Uq > Global_QPid.OutMax * radio)
		{
			Global_Foc.Uq = Global_QPid.OutMax * radio;
		}
		else if (Global_Foc.Uq < Global_QPid.OutMin * radio)
		{
			Global_Foc.Uq = Global_QPid.OutMin * radio;
		}

		// Park��任
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta,
					arm_sin_f32(Global_Motor_Cal.ElecDeg), arm_cos_f32(Global_Motor_Cal.ElecDeg));
		//�޸Ĺ���Ŀ�������任
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);
		// SVPWM ���� PWMֵ���
		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		break;
		//����״̬
	case STANDBY:
		ElecDegCal(Global_Motor_Cal.EncoderValUse);
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);
		Global_Pwm[0] = 0;
		Global_Pwm[1] = 0;
		Global_Pwm[2] = 0;
		break;
		//����״̬ ����У׼��Ƕ�0λ
	case LOCK:
		CurrentCal(0, currentdata);
		Global_Foc.Uq = 0;
		Global_Foc.Ud = MOTOCAL_KEEP_VOLTAGE;

		// Park��任
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta, arm_sin_f32(0), arm_cos_f32(0));
		//�޸Ĺ���Ŀ�������任
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);

		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		break;
		//ǿ����ת �����ڵ���Park�任�Ϳ�������任�Լ�SVPWM)
	case FORCE_TURN:
		CurrentCal(Global_Motor_Cal.ElecDeg, currentdata);
		Global_Foc.Uq = 0.0f;
		Global_Foc.Ud = MOTOCAL_KEEP_VOLTAGE;

		// Park��任
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta, arm_sin_f32(Global_Motor_Cal.ElecDeg), arm_cos_f32(Global_Motor_Cal.ElecDeg));
		//�޸Ĺ���Ŀ�������任
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);

		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		Global_Motor_Cal.ElecDeg += MOTOR_DEBUG_ROTATE_FORCE_RATE;
		break;
		//������ת�����ڵ��Ե�������
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

		// Park��任
		InvPark_f32(Global_Foc.Ud, Global_Foc.Uq, &Global_Foc.Ualpha, &Global_Foc.Ubeta, arm_sin_f32(Global_Motor_Cal.ElecDeg), arm_cos_f32(Global_Motor_Cal.ElecDeg));
		//�޸Ĺ���Ŀ�������任
		CalcRefVec(Global_Foc.Ualpha, Global_Foc.Ubeta, &Global_Foc.Ua, &Global_Foc.Ub, &Global_Foc.Uc);
		Svpwm(Global_Foc.Ua, Global_Foc.Ub, Global_Foc.Uc, MOTOR_PHASE_REPLACE, Global_Pwm);
		break;
	default:
		break;
	}
}

/***************************************************
 * ����: Svpwm
 * ����: Svpwm����
 * ����: u1,u2,u3-�����ѹ
 * ����: void
 ***************************************************/
void Svpwm(float u1, float u2, float u3, FlagStatus Rev, uint16_t *PWMX)
{

	uint8_t flagA, flagB, flagC, flagN;
	uint32_t TX, TY, Tx, Ty, Taon, Tbon, Tcon;
	uint16_t PWMX_TMP[3] = {0, 0, 0};
	uint16_t tmp = 0;
	float tmpf1;

	//�ж�����
	flagA = (u1 > 0) ? 1 : 0;
	flagB = (u2 > 0) ? 1 : 0;
	flagC = (u3 > 0) ? 1 : 0;

	flagN = flagA + (flagB << 1) + (flagC << 2);

	switch (flagN)
	{
	case 3:					   //����1
		TX = u2 * MOTO_COUNT2; //�ȷ�����ʸ��
		TY = u1 * MOTO_COUNT2; //������ʸ��

		if ((TX + TY) > MOTO_COUNT)
		{ //����������
			Tx = TX;
			Ty = TY;
			tmpf1 = 1.0f / (Tx + Ty);
			TX = Tx * (MOTO_COUNT - MOTO_NULL) * tmpf1;
			TY = Ty * (MOTO_COUNT - MOTO_NULL) * tmpf1;
		}

		Taon = (MOTO_COUNT - TX - TY) >> 1;
		Tbon = Taon + TX;
		Tcon = Tbon + TY;

		PWMX_TMP[0] = Taon; // T0(011�����ű۵�ͨ��ʱ��ռ��)
		PWMX_TMP[1] = Tbon; // T0+T4��001���ű۵�ͨ��ʱ��ռ�ȣ�
		PWMX_TMP[2] = Tcon; // T0+T4+T6��000 ���ű۵�ͨ��ʱ��ռ�ȣ�
		break;
	case 1:						//����2(ע���1������1)
		TX = -u2 * MOTO_COUNT2; //�ȷ�����ʸ��
		TY = -u3 * MOTO_COUNT2; //������ʸ��

		if ((TX + TY) > MOTO_COUNT)
		{ //����������
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
	case 5: //����3
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
	case 4: //����4
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
	case 6: //����5
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
	case 2: //����6
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

	//����
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
 * ����: SetPwmVal
 * ����: ���õ��PMM���
 * ����: val
 * ����: void
 ***************************************************/
void PWM_Set(uint16_t *val)
{
	taskDISABLE_INTERRUPTS();
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, MOTO_COUNT - val[0]);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, MOTO_COUNT - val[1]);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, MOTO_COUNT - val[2]);

	taskENABLE_INTERRUPTS();
}
