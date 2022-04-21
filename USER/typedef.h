/***************************************************
 * Description  :
 * LastEditTime : 2022-04-19 11:09:11
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\USER\typedef.h
 ***************************************************/
/*
 * @Description: typedef重定义头文件
 * @Author: mkx
 * @Date: 2022-04-11 15:51:53
 * @LastEditTime: 2022-04-12 09:23:29
 * @LastEditors: mkx
 * @FilePath: \GD32_FreeRTOS\USER\typedef.h
 * @Reference:
 */

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef unsigned int ulong;
typedef unsigned short uint;
typedef unsigned char uchar;

typedef float float32_t;

typedef enum
{
	osOK = 0,						///< function completed; no error or event occurred.
	osEventSignal = 0x08,			///< function completed; signal event occurred.
	osEventMessage = 0x10,			///< function completed; message event occurred.
	osEventMail = 0x20,				///< function completed; mail event occurred.
	osEventTimeout = 0x40,			///< function completed; timeout occurred.
	osErrorParameter = 0x80,		///< parameter error: a mandatory parameter was missing or specified an incorrect object.
	osErrorResource = 0x81,			///< resource not available: a specified resource was not available.
	osErrorTimeoutResource = 0xC1,	///< resource not available within given time: a specified resource was not available within the timeout period.
	osErrorISR = 0x82,				///< not allowed in ISR context: the function cannot be called from interrupt service routines.
	osErrorISRRecursive = 0x83,		///< function called multiple times from ISR with same object.
	osErrorPriority = 0x84,			///< system cannot determine priority or thread has illegal priority.
	osErrorNoMemory = 0x85,			///< system is out of memory: it was impossible to allocate or reserve memory for the operation.
	osErrorValue = 0x86,			///< value of a parameter is out of range.
	osErrorOS = 0xFF,				///< unspecified RTOS error: run-time error but no other error message fits.
	os_status_reserved = 0x7FFFFFFF ///< prevent from enum down-size compiler optimization.
} osStatus;

typedef struct
{
	uchar b7 : 1;
	uchar b6 : 1;
	uchar b5 : 1;
	uchar b4 : 1;
	uchar b3 : 1;
	uchar b2 : 1;
	uchar b1 : 1;
	uchar b0 : 1;
} BYTE_FIELD;

#define bool char
#define true 1
#define false 0

// 定义8位共用体
typedef union
{
	uchar ucByte;
	BYTE_FIELD stBit;
} TYPE_BYTE;

#endif
