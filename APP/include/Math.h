/***************************************************
 * Description  :
 * LastEditTime : 2022-04-15 17:26:12
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\include\Math.h
 ***************************************************/
#ifndef _MATH_H_
#define _MATH_H_

#include "UserConfig.h"

#ifndef PI
#define PI 3.14159265358979f
#endif
#define FAST_MATH_TABLE_SIZE 512

float32_t arm_sin_f32(float32_t x);
float32_t arm_cos_f32(float32_t x);

#endif
