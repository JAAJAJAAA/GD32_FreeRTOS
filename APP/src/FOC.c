/***************************************************
 * Description  : FOC��غ����ļ�
 * LastEditTime : 2022-04-16 09:06:57
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\FOC.c
 ***************************************************/
#include "FOC.h"
#include "MotorTask.h"
FOC_typedef Global_Foc;
/***************************************************
 * ����: Park_f32
 * ����: ���˱任,��ʱ����ת����
 * ����: id-d�����;iq-q�����;
 * 		ialpha-����;ibeta-����;
 * 		sinVal-��Ƕ�����ֵ;
 * 		cosVal-��Ƕ�����ֵ;
 * ����: void
 ***************************************************/
void InvPark_f32(float id, float iq, float *ialpha, float *ibeta, float sinVal, float cosVal)
{
	*ialpha = -id * cosVal + iq * sinVal;
	*ibeta = id * sinVal + iq * cosVal;
}

/***************************************************
 * ����: Park_f32
 * ����: ���˱任,˳ʱ����ת����
 * ����: ialpha-����;ibeta-����;
 * 		id-d�����;iq-q�����;
 * 		sinVal-��Ƕ�����ֵ;
 * 		cosVal-��Ƕ�����ֵ;
 * ����: void
 ***************************************************/
void Park_f32(float ialpha, float ibeta, float *pid, float *piq, float sinVal, float cosVal)
{
	*pid = -ialpha * cosVal + ibeta * sinVal;
	*piq = ialpha * sinVal + ibeta * cosVal;
}

/***************************************************
 * ����: Clark_f32
 * ����: Clark�任���Ƕ�˳��Ϊ0,120,240,ʹ�õȷ�ֵ�任
 * ����: ialpha-����;ibeta-����;
 * 		ia-a�����;ib-b�����;ic-c�����;
 * ����: void
 ***************************************************/
void InvClark_f32(float ialpha, float ibeta, float *ia, float *ib, float *ic)
{
	*ia = ialpha;
	*ib = (-ibeta * SQRT3_2 - ialpha * 0.5f);
	*ic = (ibeta * SQRT3_2 - ialpha * 0.5f);
}

/***************************************************
 * ����: Clark_f32
 * ����: Clark�任���Ƕ�˳��Ϊ0,120,240,ϵ��ʹ��2/3(�ȷ�ֵ�任)
 * ����: ia-a�����;ib-b�����;ic-c�����;
 *		ialpha-����;ibeta-����;
 * ����: void
 ***************************************************/
void Clark_f32(float ia, float ib, float ic, float *pialpha, float *ibeta)
{
	*pialpha = ia;
	*ibeta = -(0.57735026919f * ib - 0.57735026919f * ic);
}

/***************************************************
 * ����: CalcRefVec
 * ����: �޸Ĺ����Clark�任
 * ����: Ualpha��Ubeta
 *		Ua,Ub��Uc�������������ѹֵ����svpwm������
 * ����: void
 ***************************************************/
void CalcRefVec(float ialpha, float ibeta, float *ia, float *ib, float *ic)
{
	*ia = ibeta;
	*ib = (-ibeta * 0.5f - ialpha * SQRT3_2);
	*ic = (-ibeta * 0.5f + ialpha * SQRT3_2);
}
