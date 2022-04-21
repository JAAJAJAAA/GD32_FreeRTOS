
#ifndef _MOTORTASK_H_
#define _MOTORTASK_H_

//#include "UserConfig.h"
#include "FOC.h"
#include "StoreTask.h"
#include "Math.h"
#include "PID.h"
#include "MT6815.h"
#include "typedef.h"
#include "gd32f30x_timer.h"
#define USE_VOLTAGE_QPID_KP // QPID������KPֵ�Ƿ�͵�ѹֵ�����

//���ڿ��ܴ����������͵����λ����Ӧ����� ��ʱ����Ҫ���з��ദ�� ������໥�滻
#define MOTOR_PHASE_REPLACE RESET //����Ҫ�滻������ΪSET  ��Ҫ�滻������ΪRESET

#define SQRT3_DIV3 0.577350269f
#define SQRT2 1.414213562f
#define SQRT3 1.73202081f
#define SQRT3_2 0.866025403784439f

#define MOTO_COUNT 2625					 //���ÿ�����ڵ�ʱ��
#define MOTO_COUNT2 (MOTO_COUNT * SQRT3) //����ʱ�俪������

#define MOTOR_PAIR_F 11.0f //���������   ���Բ���

#define MOTOR_DEBUG_ROTATE_FORCE_RATE 0.002f //ǿ����תÿ��������
#define MOTOCAL_KEEP_VOLTAGE 0.10f			 //���������ѹ
#define MOTOCAL_FREE_TURN_VOLTAGE 0.05f		 //���������ת��ѹ 0.01
#define MOTOCAL_MAX_VOLTAGE 0.45f			 //�����������ѹ

//���������
#define ENCODER_LINE_10 (1023)
#define ENCODER_LINE_10_F (1023.0f)

#define ENCODER_LINE_11 (2047)
#define ENCODER_LINE_11_F (2047.0f)

#define ENCODER_LINE_12 (4095)
#define ENCODER_LINE_12_F (4095.0f)

#define ENCODER_LINE_14 (16383)
#define ENCODER_LINE_14_F (16383.0f)

#define ENCODER_LINE_15 (32767)
#define ENCODER_LINE_15_F (32767.0f)

#define ENCODER_LINE_16 (65535)
#define ENCODER_LINE_16_F (65535.0f)

#define ENCODER_LINE ENCODER_LINE_14
#define ENCODER_LINE_F ENCODER_LINE_14_F

#define ENCODER_LINE_DIV2 (ENCODER_LINE >> 1)
#define ENCODER_LINE_DIV2_F (ENCODER_LINE_F / 2.0f)

#define ENCODER_DEAD 2.0f //�������ı仯С�����ֵ������

#ifndef WAIT_FOREVER
#define WAIT_FOREVER 0xFFFFFFFF
#endif

#define MOTO_NULL 20

typedef struct
{
	uint16_t PWM[3]; // PWMֵ
	float Ud;
	float Uq;

	float Ualpha;
	float Ubeta;

	float Ua;
	float Ub;
	float Uc;

	float Id;
	float Iq;

	float Ialpha;
	float Ibeta;

	float Ia;
	float Ib;
	float Ic;
} FOC_typedef;

//���״̬ö��
typedef enum
{
	STANDBY = 0, //����״̬
	NORMAL,		 //����״̬
	LOCK,		 //����״̬
	FORCE_TURN,	 //ǿ����ת
	FREE_TRUN,	 //������ת

} MotorStatus_TypeDef;

typedef struct
{
	float ElecDeg;			//��Ƕ�
	uint16_t EncoderValUse; //��������ֵ
							// uint16_t CurrentRAW[2]; //����ԭʼֵ
} Moto_Cal_datatypedef;

uint16_t GetEncoder(void);
void Motor_Task(void *pvParameters);
uint32_t SemaphoreWait(SemaphoreHandle_t semaphore_id, u32 millisec);
void Motor_Task_Init(void);
void CurrentCal(float deg, uint16_t *CurrentData);
void ElecDegCal(uint16_t encoderVal);
void Svpwm(float u1, float u2, float u3, FlagStatus Rev, uint16_t *PWMX);
float CaliPhaseZero(uint16_t EncoderVal);
void SetSleep(bool isSleep);
void SetMotorStatus(MotorStatus_TypeDef status);
MotorStatus_TypeDef GetStatus(void);
float GetIq(void);
void CsON(void);
void CsOFF(void);
uint16_t ClearDR(void);

void PWM_Cal(void);
void PWM_Set(uint16_t *val);
#endif
