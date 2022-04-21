
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
#define USE_VOLTAGE_QPID_KP // QPID电流环KP值是否和电压值负相关

//由于可能存在软件输出和电机相位不对应的情况 这时候需要进行反相处理 相输出相互替换
#define MOTOR_PHASE_REPLACE RESET //不需要替换则设置为SET  需要替换则设置为RESET

#define SQRT3_DIV3 0.577350269f
#define SQRT2 1.414213562f
#define SQRT3 1.73202081f
#define SQRT3_2 0.866025403784439f

#define MOTO_COUNT 2625					 //电机每个周期的时间
#define MOTO_COUNT2 (MOTO_COUNT * SQRT3) //周期时间开立方根

#define MOTOR_PAIR_F 11.0f //电机极对数   可以测量

#define MOTOR_DEBUG_ROTATE_FORCE_RATE 0.002f //强制旋转每周期增量
#define MOTOCAL_KEEP_VOLTAGE 0.10f			 //电机锁定电压
#define MOTOCAL_FREE_TURN_VOLTAGE 0.05f		 //电机自由旋转电压 0.01
#define MOTOCAL_MAX_VOLTAGE 0.45f			 //电机最大输出电压

//编码器相关
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

#define ENCODER_DEAD 2.0f //编码器的变化小于这个值，忽略

#ifndef WAIT_FOREVER
#define WAIT_FOREVER 0xFFFFFFFF
#endif

#define MOTO_NULL 20

typedef struct
{
	uint16_t PWM[3]; // PWM值
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

//电机状态枚举
typedef enum
{
	STANDBY = 0, //待机状态
	NORMAL,		 //正常状态
	LOCK,		 //锁定状态
	FORCE_TURN,	 //强制旋转
	FREE_TRUN,	 //自由旋转

} MotorStatus_TypeDef;

typedef struct
{
	float ElecDeg;			//电角度
	uint16_t EncoderValUse; //编码器数值
							// uint16_t CurrentRAW[2]; //电流原始值
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
