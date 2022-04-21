/***************************************************
 * Description  : FOC相关函数文件
 * LastEditTime : 2022-04-16 09:06:57
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\FOC.c
 ***************************************************/
#include "FOC.h"
#include "MotorTask.h"
FOC_typedef Global_Foc;
/***************************************************
 * 名称: Park_f32
 * 描述: 帕克变换,逆时针旋转方向
 * 参数: id-d轴电流;iq-q轴电流;
 * 		ialpha-α轴;ibeta-β轴;
 * 		sinVal-电角度正弦值;
 * 		cosVal-电角度余弦值;
 * 返回: void
 ***************************************************/
void InvPark_f32(float id, float iq, float *ialpha, float *ibeta, float sinVal, float cosVal)
{
	*ialpha = -id * cosVal + iq * sinVal;
	*ibeta = id * sinVal + iq * cosVal;
}

/***************************************************
 * 名称: Park_f32
 * 描述: 帕克变换,顺时针旋转方向
 * 参数: ialpha-α轴;ibeta-β轴;
 * 		id-d轴电流;iq-q轴电流;
 * 		sinVal-电角度正弦值;
 * 		cosVal-电角度余弦值;
 * 返回: void
 ***************************************************/
void Park_f32(float ialpha, float ibeta, float *pid, float *piq, float sinVal, float cosVal)
{
	*pid = -ialpha * cosVal + ibeta * sinVal;
	*piq = ialpha * sinVal + ibeta * cosVal;
}

/***************************************************
 * 名称: Clark_f32
 * 描述: Clark变换，角度顺序为0,120,240,使用等幅值变换
 * 参数: ialpha-α轴;ibeta-β轴;
 * 		ia-a相电流;ib-b相电流;ic-c相电流;
 * 返回: void
 ***************************************************/
void InvClark_f32(float ialpha, float ibeta, float *ia, float *ib, float *ic)
{
	*ia = ialpha;
	*ib = (-ibeta * SQRT3_2 - ialpha * 0.5f);
	*ic = (ibeta * SQRT3_2 - ialpha * 0.5f);
}

/***************************************************
 * 名称: Clark_f32
 * 描述: Clark变换，角度顺序为0,120,240,系数使用2/3(等幅值变换)
 * 参数: ia-a相电流;ib-b相电流;ic-c相电流;
 *		ialpha-α轴;ibeta-β轴;
 * 返回: void
 ***************************************************/
void Clark_f32(float ia, float ib, float ic, float *pialpha, float *ibeta)
{
	*pialpha = ia;
	*ibeta = -(0.57735026919f * ib - 0.57735026919f * ic);
}

/***************************************************
 * 名称: CalcRefVec
 * 描述: 修改过后的Clark变换
 * 参数: Ualpha和Ubeta
 *		Ua,Ub和Uc是输出的三个电压值，是svpwm的输入
 * 返回: void
 ***************************************************/
void CalcRefVec(float ialpha, float ibeta, float *ia, float *ib, float *ic)
{
	*ia = ibeta;
	*ib = (-ibeta * 0.5f - ialpha * SQRT3_2);
	*ic = (-ibeta * 0.5f + ialpha * SQRT3_2);
}
