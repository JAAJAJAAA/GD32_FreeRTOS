#ifndef _FOC_H_
#define _FOC_H_

#include "UserConfig.h"

#define SQRT3_DIV3 0.577350269f
#define SQRT2 1.414213562f
#define SQRT3 1.73202081f
#define SQRT3_2 0.866025403784439f

void InvPark_f32(float id, float iq, float *ialpha, float *ibeta, float sinVal, float cosVal);
void Park_f32(float ialpha, float ibeta, float *pid, float *piq, float sinVal, float cosVal);
void InvClark_f32(float ialpha, float ibeta, float *ia, float *ib, float *ic);
void Clark_f32(float ia, float ib, float ic, float *pialpha, float *ibeta);
void CalcRefVec(float ialpha, float ibeta, float *ia, float *ib, float *ic);

#endif
