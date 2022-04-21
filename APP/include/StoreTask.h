/***************************************************
 * Description  :
 * LastEditTime : 2022-04-19 11:09:19
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\include\StoreTask.h
 ***************************************************/
#ifndef _STORETASK_H_
#define _STORETASK_H_

//#include "UserConfig.h"

#include "typedef.h"
#include "MotorTask.h"
//因为EEPROM 页面为16byte 所以地址的起始必须为16byte整数倍
//以下的LEN是包含数据长度和校验长度的最大空间长度
//相位参数
#define STORE_MOTOR_CALI_PARAM_BASE_ADDR (0u)

//行程校准参数
#define STORE_TRAVE_PARAM_BASE_ADDR ((4096) + (16))

//当前行程参数
#define STORE_TRAVE_CURRENT_BASE_ADDR ((4096) + (32))

//用户参数
#define STORE_USER_PARAM_BASE_ADDR (8192)

//轴类型参数
#define STORE_LENS_TYPE_BASE_ADDR (12288)

#define osFeature_MailQ 1 ///< Mail Queues:     1=available, 0=not available
#define inHandlerMode() __get_IPSR() != 0

typedef struct os_pool_cb *osPoolId;
typedef struct os_mailQ_cb *osMailQId;

typedef enum
{
    STORE_MOTOR_PARAM, //电机校准参数
    STORE_USER_PARAM,  //用户参数
    STORE_TRAVE_PARAM, //行程参数
    STORE_TURN_NUM,    //旋转圈数
    STORE_LENS_TYPE,   //轴类型
} StoreType_EnumDef;

//电机校准参数
typedef struct
{
    float PhaseParam;
    int16_t CurrentOffset[2];
} MotorCaliParam_TypeDef;

//镜头类型
#pragma pack(4)
typedef enum
{
    M_FOCUS = 0, //跟焦
    M_ZOOM,      //焦点
    M_IRIS,      //光圈
} LenType_TypeDef;

//用户参数
#pragma pack(4)
typedef struct
{
    uint8_t Torque;
    uint8_t Speed;
    bool IsMoutL; //安装方向
} UserParam_TypdeDef;

//电机行程数据
typedef struct
{
    float StartPoint;    //起始点
    float EndPoint;      //结束点
    float TotalLenth;    //总行程长度
    int8_t TotalTurnNum; //圈数
} TraveParam_TypeDef;

typedef struct
{
    int8_t TurnNum;      //圈数
    float CurrentLenth;  //当前行程
    float ManuCaliLenth; //进入手动校准时刻行程

} TraveData_TypeDef;

typedef struct
{
    bool IsCaliComplete; //校准是否完成
} TraveFlag_TypeDef;

//跟焦电机行程数据
typedef struct
{
    TraveFlag_TypeDef Flag;   //标志
    TraveParam_TypeDef Param; //参数
    TraveData_TypeDef Data;   //数据
} Trave_TypeDef;
//当前行程数据
typedef struct
{
    int8_t TurnNum; //旋转圈数
} TurnNum_TypeDef;

//全部参数
typedef struct
{
    //电机校准参数
    MotorCaliParam_TypeDef MotorCaliParam;
    //轴类型参数
    LenType_TypeDef LenType;
    //用户参数
    UserParam_TypdeDef UserParam;
    //跟焦电机行程校准参数
    TraveParam_TypeDef TraveParam;
    //当前旋转圈数
    TurnNum_TypeDef CurrentTurnNum;
} Store_ParamAllTypeDef;

union Store_ParamStatusTypeDef
{
    struct
    {
        // 0故障 1正常
        uint16_t MotorCaliParamStatus : 1;
        uint16_t LenTypeParamStatus : 1;
        uint16_t UserParamStatus : 1;
        uint16_t TraveParamStatus : 1;
    } Bit;
    uint16_t All;
};

typedef struct
{
    uint8_t Store_type;      //存储数据类型
    uint16_t len;            //数据长度
    uint8_t *Store_data_ptr; //数据指针

} StoreMsgTypeDef;

//单字节对齐
#pragma pack(1)
typedef union
{
    struct
    {
        uint16_t OperateMode : 2;       //操作模式
        uint16_t MotorStatus : 3;       //电机状态
        uint16_t MemoryMotorStatus : 1; //参数存储状态
        uint16_t MemoryUserStatus : 1;  //参数存储状态
        uint16_t MemoryTraveStatus : 1; //参数存储状态
        uint16_t ABPointStatus : 2;     // AB点设置情况
        uint16_t Reserved : 6;
    } bit;
    uint16_t data;
} Status_TypeDef;
#pragma pack()

typedef struct os_pool_cb
{
    void *pool;
    uint8_t *markers;
    uint32_t pool_sz;
    uint32_t item_sz;
    uint32_t currentIndex;
} os_pool_cb_t;
typedef struct os_mailQ_def
{
    uint32_t queue_sz; ///< number of elements in the queue
    uint32_t item_sz;  ///< size of an item
    struct os_mailQ_cb **cb;
} osMailQDef_t;

typedef struct os_mailQ_cb
{
    const osMailQDef_t *queue_def;
    QueueHandle_t handle;
    osPoolId pool;
} os_mailQ_cb_t;

typedef struct os_pool_def
{
    uint32_t pool_sz; ///< number of items (elements) in the pool
    uint32_t item_sz; ///< size of an item
    void *pool;       ///< pointer to memory for pool
} osPoolDef_t;

typedef struct
{
    osStatus status; ///< status code: event or error information
    union
    {
        uint32_t v;      ///< message as 32-bit value
        void *p;         ///< message or mail as void pointer
        int32_t signals; ///< signal flags
    } value;             ///< event value
    union
    {
        osMailQId mail_id;        ///< mail id obtained by \ref osMailCreate
        QueueHandle_t message_id; ///< message id obtained by \ref osMessageCreate
    } def;                        ///< event definition
} osEvent;

typedef enum
{
    CW,  //电机顺时针旋转
    CCW, //电机逆时针旋转
} MotoDir_Enum;

extern osMailQId Global_StoreMailhandle;
extern TaskHandle_t Global_Store_Task_Handle;

extern Status_TypeDef Global_Store_Status;
osStatus osPoolFree(osPoolId pool_id, void *block);
osStatus osMailPut(osMailQId queue_id, void *mail);
osEvent osMailGet(osMailQId queue_id, uint32_t millisec);
void *osPoolAlloc(osPoolId pool_id);
void Store_mail_put(StoreType_EnumDef type, uint8_t *src, uint16_t len);
bool ReadParam(StoreType_EnumDef type);
void Store_Turn_num(void);
int8_t Get_Store_TurnNum(void);
UserParam_TypdeDef Get_Store_UserParam(void);
bool Store_Param_Cheak(StoreType_EnumDef Type);
void Store_Task_Init(void);
osMailQId osMailCreate(const osMailQDef_t *queue_def, osThreadId thread_id);
void StoreTask(void *pvParameters);
#endif
