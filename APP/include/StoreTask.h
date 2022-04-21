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
//��ΪEEPROM ҳ��Ϊ16byte ���Ե�ַ����ʼ����Ϊ16byte������
//���µ�LEN�ǰ������ݳ��Ⱥ�У�鳤�ȵ����ռ䳤��
//��λ����
#define STORE_MOTOR_CALI_PARAM_BASE_ADDR (0u)

//�г�У׼����
#define STORE_TRAVE_PARAM_BASE_ADDR ((4096) + (16))

//��ǰ�г̲���
#define STORE_TRAVE_CURRENT_BASE_ADDR ((4096) + (32))

//�û�����
#define STORE_USER_PARAM_BASE_ADDR (8192)

//�����Ͳ���
#define STORE_LENS_TYPE_BASE_ADDR (12288)

#define osFeature_MailQ 1 ///< Mail Queues:     1=available, 0=not available
#define inHandlerMode() __get_IPSR() != 0

typedef struct os_pool_cb *osPoolId;
typedef struct os_mailQ_cb *osMailQId;

typedef enum
{
    STORE_MOTOR_PARAM, //���У׼����
    STORE_USER_PARAM,  //�û�����
    STORE_TRAVE_PARAM, //�г̲���
    STORE_TURN_NUM,    //��תȦ��
    STORE_LENS_TYPE,   //������
} StoreType_EnumDef;

//���У׼����
typedef struct
{
    float PhaseParam;
    int16_t CurrentOffset[2];
} MotorCaliParam_TypeDef;

//��ͷ����
#pragma pack(4)
typedef enum
{
    M_FOCUS = 0, //����
    M_ZOOM,      //����
    M_IRIS,      //��Ȧ
} LenType_TypeDef;

//�û�����
#pragma pack(4)
typedef struct
{
    uint8_t Torque;
    uint8_t Speed;
    bool IsMoutL; //��װ����
} UserParam_TypdeDef;

//����г�����
typedef struct
{
    float StartPoint;    //��ʼ��
    float EndPoint;      //������
    float TotalLenth;    //���г̳���
    int8_t TotalTurnNum; //Ȧ��
} TraveParam_TypeDef;

typedef struct
{
    int8_t TurnNum;      //Ȧ��
    float CurrentLenth;  //��ǰ�г�
    float ManuCaliLenth; //�����ֶ�У׼ʱ���г�

} TraveData_TypeDef;

typedef struct
{
    bool IsCaliComplete; //У׼�Ƿ����
} TraveFlag_TypeDef;

//��������г�����
typedef struct
{
    TraveFlag_TypeDef Flag;   //��־
    TraveParam_TypeDef Param; //����
    TraveData_TypeDef Data;   //����
} Trave_TypeDef;
//��ǰ�г�����
typedef struct
{
    int8_t TurnNum; //��תȦ��
} TurnNum_TypeDef;

//ȫ������
typedef struct
{
    //���У׼����
    MotorCaliParam_TypeDef MotorCaliParam;
    //�����Ͳ���
    LenType_TypeDef LenType;
    //�û�����
    UserParam_TypdeDef UserParam;
    //��������г�У׼����
    TraveParam_TypeDef TraveParam;
    //��ǰ��תȦ��
    TurnNum_TypeDef CurrentTurnNum;
} Store_ParamAllTypeDef;

union Store_ParamStatusTypeDef
{
    struct
    {
        // 0���� 1����
        uint16_t MotorCaliParamStatus : 1;
        uint16_t LenTypeParamStatus : 1;
        uint16_t UserParamStatus : 1;
        uint16_t TraveParamStatus : 1;
    } Bit;
    uint16_t All;
};

typedef struct
{
    uint8_t Store_type;      //�洢��������
    uint16_t len;            //���ݳ���
    uint8_t *Store_data_ptr; //����ָ��

} StoreMsgTypeDef;

//���ֽڶ���
#pragma pack(1)
typedef union
{
    struct
    {
        uint16_t OperateMode : 2;       //����ģʽ
        uint16_t MotorStatus : 3;       //���״̬
        uint16_t MemoryMotorStatus : 1; //�����洢״̬
        uint16_t MemoryUserStatus : 1;  //�����洢״̬
        uint16_t MemoryTraveStatus : 1; //�����洢״̬
        uint16_t ABPointStatus : 2;     // AB���������
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
    CW,  //���˳ʱ����ת
    CCW, //�����ʱ����ת
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
