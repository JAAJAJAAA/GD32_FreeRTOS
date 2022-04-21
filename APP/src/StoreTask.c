/***************************************************
 * Description  :
 * LastEditTime : 2022-04-19 11:06:36
 * LastEditors  : mkx
 * FilePath     : \GD32_FreeRTOS\APP\src\StoreTask.c
 ***************************************************/
#include "StoreTask.h"

osPoolId osPoolCreate(const osPoolDef_t *pool_def);

TaskHandle_t Global_Store_Task_Handle;        //存储任务句柄
osMailQId Global_StoreMailhandle;             //存储邮箱句柄
osPoolId Global_Store_PoolHandle;             //存储数据用缓存句柄
osPoolId Global_Store_Ptr_PoolHandle;         //存储数据指针缓存句柄
MotorCaliParam_TypeDef Global_MotorCaliParam; //电机校准参数
Store_ParamAllTypeDef Global_Store_All_Param; //所有参数
Status_TypeDef Global_Store_Status;           //参数存储状态
extern Trave_TypeDef Global_Trave;//校准完成后行程
/***************************************************
 * 名称: UserParam_TypdeDef
 * 描述: 获取用户参数
 * 参数: void
 * 返回: UserParam_TypdeDef 用户参数
 ***************************************************/
UserParam_TypdeDef Get_Store_UserParam(void)
{
    return Global_Store_All_Param.UserParam;
}

/***************************************************
 * 名称: tore_Task_Init
 * 描述: Store任务初始化
 * 参数: void
 * 返回: void
 ***************************************************/
void Store_Task_Init()
{
    // 创建存储消息邮箱
    struct os_mailQ_cb *os_mailQ_cb_Store_Mail_data;
    const osMailQDef_t os_mailQ_def_Store_Mail_data = {(100), sizeof(StoreMsgTypeDef), (&os_mailQ_cb_Store_Mail_data)};
    Global_StoreMailhandle = osMailCreate(&os_mailQ_def_Store_Mail_data, NULL);
    // 创建消息数据缓冲空间
    const osPoolDef_t os_pool_def_Store = {100, sizeof(StoreMsgTypeDef), NULL};
    Global_Store_PoolHandle = osPoolCreate(&os_pool_def_Store);
    // 创建消息指针缓冲空间
    const osPoolDef_t os_pool_def_Store_Ptr = {100, sizeof(uint8_t), NULL};
    Global_Store_Ptr_PoolHandle = osPoolCreate(&os_pool_def_Store_Ptr);

    // 创建存储任务
    xTaskCreate((TaskFunction_t)StoreTask,                  //任务函数
                (const char *)"StoreTask",                  //任务名称
                (uint16_t)configMINIMAL_STACK_SIZE,         //任务堆栈大小
                (void *)NULL,                               //传递给任务函数的参数
                (UBaseType_t)1,                             //任务优先级
                (TaskHandle_t *)&Global_Store_Task_Handle); //任务句柄
    printf(" The Store Task initialization succeeded \r\n");

    // 关闭写保护
    // WpCmd(false);
}

void StoreTask(void *pvParameters)
{
    StoreMsgTypeDef *tmp = NULL;
    osEvent event;
    while (1)
    {
        event = osMailGet(Global_StoreMailhandle, WAIT_FOREVER);
        if (event.status == osEventMail)
        {
            tmp = (StoreMsgTypeDef *)event.value.p;
            switch ((StoreType_EnumDef)tmp->Store_type)
            {
            case STORE_MOTOR_PARAM: //电机校准数据
                // Write_Data_HAL(STORE_MOTOR_CALI_PARAM_BASE_ADDR, tmp->Store_data_ptr, tmp->len);
                break;
            case STORE_USER_PARAM: //存储用户参数
                // Write_Data_HAL(STORE_USER_PARAM_BASE_ADDR, tmp->Store_data_ptr, tmp->len);
                break;
            case STORE_TRAVE_PARAM: //存储行程参数
                // Write_Data_HAL(STORE_TRAVE_PARAM_BASE_ADDR, tmp->Store_data_ptr, tmp->len);
                break;
            case STORE_TURN_NUM: //存储圈数
                // Write_Data_HAL(STORE_TRAVE_CURRENT_BASE_ADDR, tmp->Store_data_ptr, tmp->len);
                break;
            case STORE_LENS_TYPE: //存储轴类型
                // Write_Data_HAL(STORE_LENS_TYPE_BASE_ADDR, tmp->Store_data_ptr, tmp->len);
                break;
            default:
                break;
            }
        }
    }
}

/***************************************************
名字:	Store_mail_put
参数:	StoreType_EnumDef type
     ...存储的电机数据类型
     uint8_t*src
     ...数据源
     uint16_t len
     ...数据长度
功能:	发送待存储的数据到邮箱
返回:	void
***************************************************/
void Store_mail_put(StoreType_EnumDef type, uint8_t *src, uint16_t len)
{
    StoreMsgTypeDef *tmp = NULL;
    uint8_t *Ptr = (uint8_t *)osPoolAlloc(Global_Store_Ptr_PoolHandle); //申请内存空间
    if (Ptr != NULL)
    {
        tmp = (StoreMsgTypeDef *)osPoolAlloc(Global_Store_PoolHandle); //申请内存空间
        if (tmp == NULL)
        {
            osPoolFree(Global_Store_PoolHandle, tmp); //释放内存空间
        }
        else
        {

            tmp->Store_type = (uint8_t)type;
            tmp->Store_data_ptr = Ptr;
        }
    }
    else
    {
        osPoolFree(Global_Store_Ptr_PoolHandle, Ptr);
    }
    memcpy(tmp->Store_data_ptr, src, len);
    tmp->len = len;
    taskDISABLE_INTERRUPTS(); //关闭中断防止被打断
    osMailPut(Global_StoreMailhandle, tmp);
    osPoolFree(Global_Store_PoolHandle, tmp);
    osPoolFree(Global_Store_Ptr_PoolHandle, Ptr);
    taskENABLE_INTERRUPTS(); //开启中断
}
/***************************************************
名字:	ReadParam
参数:	StoreType_EnumDef type
                ...读取的参数类型
            uint32_t id
                ...读取的参数ID
功能:	读取参数
说明:	函数在读取数据后会进行参数检查
返回:	bool false 读取失败/参数检查错误
***************************************************/
bool ReadParam(StoreType_EnumDef type)
{
    bool result = false;

    switch (type)
    {

    case STORE_MOTOR_PARAM: //读取电机参数
        // Read_Data_HAL(STORE_MOTOR_CALI_PARAM_BASE_ADDR, (uint8_t *)&Global_MotorCaliParam, sizeof(MotorCaliParam_TypeDef));
        result = Store_Param_Cheak(type);
        if (result)
        {
            Global_Store_Status.bit.MemoryMotorStatus = true;
        }
        else
        {
            Global_Store_Status.bit.MemoryMotorStatus = false;
        }
        break;

    case STORE_USER_PARAM: //读取用户参数
        // Read_Data_HAL(STORE_USER_PARAM_BASE_ADDR, (uint8_t *)&Global_Store_All_Param.UserParam, sizeof(UserParam_TypdeDef));
        result = Store_Param_Cheak(type);
        if (result)
        {
            Global_Store_Status.bit.MemoryUserStatus = true;
        }
        else
        {
            Global_Store_Status.bit.MemoryUserStatus = false;
        }
        break;

    case STORE_TRAVE_PARAM: //读取行程参数
        // Read_Data_HAL(STORE_TRAVE_PARAM_BASE_ADDR, (uint8_t *)&Global_Trave.Param, sizeof(TraveParam_TypeDef));
        result = Store_Param_Cheak(type);
        if (result)
        {
            Global_Store_Status.bit.MemoryTraveStatus = true;
        }
        else
        {
            Global_Store_Status.bit.MemoryTraveStatus = false;
        }
        break;

    case STORE_TURN_NUM: //读取圈数
        // Read_Data_HAL(STORE_TRAVE_CURRENT_BASE_ADDR, (uint8_t *)&Global_Store_All_Param.CurrentTurnNum.TurnNum, sizeof(int8_t));
        result = Store_Param_Cheak(type);
        break;
    case STORE_LENS_TYPE: //轴类型
        // Read_Data_HAL(STORE_LENS_TYPE_BASE_ADDR, (uint8_t *)&Global_Store_All_Param.LenType, sizeof(LenType_TypeDef));
        result = Store_Param_Cheak(type);
        break;
    default:
        break;
    }
    return result;
}
/***************************************************
名字:	Get_Store_TurnNum
参数:	void
功能:	返回存储的旋转圈数值
返回:	int8_t 圈数值
***************************************************/
int8_t Get_Store_TurnNum(void)
{

    return Global_Store_All_Param.CurrentTurnNum.TurnNum;
}

/***************************************************
名字:	Param_Data_Cheak
参数:StoreType_EnumDef Type
功能:	检查参数是否错误
返回:	bool
***************************************************/
bool Store_Param_Cheak(StoreType_EnumDef Type)
{
    bool result = true;
    //检查参数是否错误
    switch (Type)
    {
    case STORE_MOTOR_PARAM:
        if (Global_MotorCaliParam.PhaseParam > ENCODER_LINE_F / MOTOR_PAIR_F)
        {
            Global_MotorCaliParam.PhaseParam = 0;
            result = false;
        }
        if (Global_MotorCaliParam.CurrentOffset[0] > 100)
        {

            Global_MotorCaliParam.CurrentOffset[0] = 0;
            result = false;
        }
        if (Global_MotorCaliParam.CurrentOffset[1] > 100)
        {

            Global_MotorCaliParam.CurrentOffset[1] = 0;
            result = false;
        }
        break;
    //用户参数
    case STORE_USER_PARAM:
        //检查用户设置的最大力矩参数
        if (Global_Store_All_Param.UserParam.Torque > 100)
        {
            Global_Store_All_Param.UserParam.Torque = 50;
            result = false;
        }
        //检查用户设置的最大速度参数
        if (Global_Store_All_Param.UserParam.Speed > 100)
        {
            Global_Store_All_Param.UserParam.Speed = 50;
            result = false;
        }
        //检查安装方向参数
        if (((MotoDir_Enum)Global_Store_All_Param.UserParam.IsMoutL != CCW) && ((MotoDir_Enum)Global_Store_All_Param.UserParam.IsMoutL != CW))
        {
            Global_Store_All_Param.UserParam.IsMoutL = CCW;
            result = false;
        }
        break;
    //行程参数
    case STORE_TRAVE_PARAM:
        if (Global_Trave.Param.StartPoint > ENCODER_LINE_F)
        {
            Global_Trave.Param.StartPoint = 0.0f;
            Global_Trave.Param.EndPoint = 0.0f;
            Global_Trave.Param.TotalTurnNum = 0;
            Global_Trave.Param.TotalLenth = 0.0f;
            result = false;
        }
        if (Global_Trave.Param.EndPoint > ENCODER_LINE_F)
        {
            Global_Trave.Param.StartPoint = 0.0f;
            Global_Trave.Param.EndPoint = 0.0f;
            Global_Trave.Param.TotalTurnNum = 0;
            Global_Trave.Param.TotalLenth = 0.0f;
            result = false;
        }
        break;
        //圈数参数不受限制
    case STORE_TURN_NUM:
        break;
    //轴类型只能是 FOCUS  ZOOM  IRIS
    case STORE_LENS_TYPE:
        if ((Global_Store_All_Param.LenType == M_FOCUS) || (Global_Store_All_Param.LenType == M_ZOOM) || (Global_Store_All_Param.LenType == M_IRIS))
        {
            result = true;
        }
        else
        {
            Global_Store_All_Param.LenType = M_FOCUS;
            result = false;
        }
        break;
    default:
        result = false;
        break;
    }
    return result;
}

/********************   Mail Queue Management Functions  ***********************/
#if (defined(osFeature_MailQ) && (osFeature_MailQ != 0)) /* Use Mail Queues */

/**
 * @brief Create and Initialize a memory pool
 * @param  pool_def      memory pool definition referenced with \ref osPool.
 * @retval  memory pool ID for reference by other functions or NULL in case of error.
 * @note   MUST REMAIN UNCHANGED: \b osPoolCreate shall be consistent in every CMSIS-RTOS.
 */
osPoolId osPoolCreate(const osPoolDef_t *pool_def)
{
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
    osPoolId thePool;
    int itemSize = 4 * ((pool_def->item_sz + 3) / 4);
    uint32_t i;

    /* First have to allocate memory for the pool control block. */
    thePool = pvPortMalloc(sizeof(os_pool_cb_t));

    if (thePool)
    {
        thePool->pool_sz = pool_def->pool_sz;
        thePool->item_sz = itemSize;
        thePool->currentIndex = 0;

        /* Memory for markers */
        thePool->markers = pvPortMalloc(pool_def->pool_sz);

        if (thePool->markers)
        {
            /* Now allocate the pool itself. */
            thePool->pool = pvPortMalloc(pool_def->pool_sz * itemSize);

            if (thePool->pool)
            {
                for (i = 0; i < pool_def->pool_sz; i++)
                {
                    thePool->markers[i] = 0;
                }
            }
            else
            {
                vPortFree(thePool->markers);
                vPortFree(thePool);
                thePool = NULL;
            }
        }
        else
        {
            vPortFree(thePool);
            thePool = NULL;
        }
    }

    return thePool;

#else
    return NULL;
#endif
}

/**
 * @brief Create and Initialize mail queue
 * @param  queue_def     reference to the mail queue definition obtain with \ref osMailQ
 * @param   thread_id     thread ID (obtained by \ref osThreadCreate or \ref osThreadGetId) or NULL.
 * @retval mail queue ID for reference by other functions or NULL in case of error.
 * @note   MUST REMAIN UNCHANGED: \b osMailCreate shall be consistent in every CMSIS-RTOS.
 */
osMailQId osMailCreate(const osMailQDef_t *queue_def, osThreadId thread_id)
{
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
    //(void)thread_id;

    osPoolDef_t pool_def = {queue_def->queue_sz, queue_def->item_sz, NULL};

    /* Create a mail queue control block */

    *(queue_def->cb) = pvPortMalloc(sizeof(struct os_mailQ_cb));

    if (*(queue_def->cb) == NULL)
    {
        return NULL;
    }
    (*(queue_def->cb))->queue_def = queue_def;

    /* Create a queue in FreeRTOS */
    (*(queue_def->cb))->handle = xQueueCreate(queue_def->queue_sz, sizeof(void *));

    if ((*(queue_def->cb))->handle == NULL)
    {
        vPortFree(*(queue_def->cb));
        return NULL;
    }

    /* Create a mail pool */
    (*(queue_def->cb))->pool = osPoolCreate(&pool_def);
    if ((*(queue_def->cb))->pool == NULL)
    {
        // TODO: Delete queue. How to do it in FreeRTOS?
        vPortFree(*(queue_def->cb));
        return NULL;
    }

    return *(queue_def->cb);
#else
    return NULL;
#endif
}

/**
 * @brief Allocate a memory block from a memory pool
 * @param pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
 * @retval  address of the allocated memory block or NULL in case of no memory available.
 * @note   MUST REMAIN UNCHANGED: \b osPoolAlloc shall be consistent in every CMSIS-RTOS.
 */
void *osPoolAlloc(osPoolId pool_id)
{
    int dummy = 0;
    void *p = NULL;
    uint32_t i;
    uint32_t index;

    if (inHandlerMode())
    {
        dummy = portSET_INTERRUPT_MASK_FROM_ISR();
    }
    else
    {
        vPortEnterCritical();
    }

    for (i = 0; i < pool_id->pool_sz; i++)
    {
        index = (pool_id->currentIndex + i) % pool_id->pool_sz;

        if (pool_id->markers[index] == 0)
        {
            pool_id->markers[index] = 1;
            p = (void *)((uint32_t)(pool_id->pool) + (index * pool_id->item_sz));
            pool_id->currentIndex = index;
            break;
        }
    }

    if (inHandlerMode())
    {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(dummy);
    }
    else
    {
        vPortExitCritical();
    }

    return p;
}

/**
 * @brief Allocate a memory block from a mail
 * @param  queue_id      mail queue ID obtained with \ref osMailCreate.
 * @param  millisec      timeout value or 0 in case of no time-out.
 * @retval pointer to memory block that can be filled with mail or NULL in case error.
 * @note   MUST REMAIN UNCHANGED: \b osMailAlloc shall be consistent in every CMSIS-RTOS.
 */
void *osMailAlloc(osMailQId queue_id, uint32_t millisec)
{
    (void)millisec;
    void *p;

    if (queue_id == NULL)
    {
        return NULL;
    }

    p = osPoolAlloc(queue_id->pool);

    return p;
}

/**
 * @brief Allocate a memory block from a mail and set memory block to zero
 * @param  queue_id      mail queue ID obtained with \ref osMailCreate.
 * @param  millisec      timeout value or 0 in case of no time-out.
 * @retval pointer to memory block that can be filled with mail or NULL in case error.
 * @note   MUST REMAIN UNCHANGED: \b osMailCAlloc shall be consistent in every CMSIS-RTOS.
 */
void *osMailCAlloc(osMailQId queue_id, uint32_t millisec)
{
    uint32_t i;
    void *p = osMailAlloc(queue_id, millisec);

    if (p)
    {
        for (i = 0; i < queue_id->queue_def->item_sz; i++)
        {
            ((uint8_t *)p)[i] = 0;
        }
    }

    return p;
}

/**
 * @brief Put a mail to a queue
 * @param  queue_id      mail queue ID obtained with \ref osMailCreate.
 * @param  mail          memory block previously allocated with \ref osMailAlloc or \ref osMailCAlloc.
 * @retval status code that indicates the execution status of the function.
 * @note   MUST REMAIN UNCHANGED: \b osMailPut shall be consistent in every CMSIS-RTOS.
 */
osStatus osMailPut(osMailQId queue_id, void *mail)
{
    portBASE_TYPE taskWoken;

    if (queue_id == NULL)
    {
        return osErrorParameter;
    }

    taskWoken = pdFALSE;

    if (inHandlerMode())
    {
        if (xQueueSendFromISR(queue_id->handle, &mail, &taskWoken) != pdTRUE)
        {
            return osErrorOS;
        }
        portEND_SWITCHING_ISR(taskWoken);
    }
    else
    {
        if (xQueueSend(queue_id->handle, &mail, 0) != pdTRUE)
        {
            return osErrorOS;
        }
    }

    return osOK;
}

/**
 * @brief Get a mail from a queue
 * @param  queue_id   mail queue ID obtained with \ref osMailCreate.
 * @param millisec    timeout value or 0 in case of no time-out
 * @retval event that contains mail information or error code.
 * @note   MUST REMAIN UNCHANGED: \b osMailGet shall be consistent in every CMSIS-RTOS.
 */
osEvent osMailGet(osMailQId queue_id, uint32_t millisec)
{
    portBASE_TYPE taskWoken;
    TickType_t ticks;
    osEvent event;

    event.def.mail_id = queue_id;

    if (queue_id == NULL)
    {
        event.status = osErrorParameter;
        return event;
    }

    taskWoken = pdFALSE;

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

    if (inHandlerMode())
    {
        if (xQueueReceiveFromISR(queue_id->handle, &event.value.p, &taskWoken) == pdTRUE)
        {
            /* We have mail */
            event.status = osEventMail;
        }
        else
        {
            event.status = osOK;
        }
        portEND_SWITCHING_ISR(taskWoken);
    }
    else
    {
        if (xQueueReceive(queue_id->handle, &event.value.p, ticks) == pdTRUE)
        {
            /* We have mail */
            event.status = osEventMail;
        }
        else
        {
            event.status = (ticks == 0) ? osOK : osEventTimeout;
        }
    }

    return event;
}
/**
 * @brief Return an allocated memory block back to a specific memory pool
 * @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
 * @param  block         address of the allocated memory block that is returned to the memory pool.
 * @retval  status code that indicates the execution status of the function.
 * @note   MUST REMAIN UNCHANGED: \b osPoolFree shall be consistent in every CMSIS-RTOS.
 */
osStatus osPoolFree(osPoolId pool_id, void *block)
{
    uint32_t index;

    if (pool_id == NULL)
    {
        return osErrorParameter;
    }

    if (block == NULL)
    {
        return osErrorParameter;
    }

    if (block < pool_id->pool)
    {
        return osErrorParameter;
    }

    index = (uint32_t)block - (uint32_t)(pool_id->pool);
    if (index % pool_id->item_sz)
    {
        return osErrorParameter;
    }
    index = index / pool_id->item_sz;
    if (index >= pool_id->pool_sz)
    {
        return osErrorParameter;
    }

    pool_id->markers[index] = 0;

    return osOK;
}

/**
 * @brief Free a memory block from a mail
 * @param  queue_id mail queue ID obtained with \ref osMailCreate.
 * @param  mail     pointer to the memory block that was obtained with \ref osMailGet.
 * @retval status code that indicates the execution status of the function.
 * @note   MUST REMAIN UNCHANGED: \b osMailFree shall be consistent in every CMSIS-RTOS.
 */
osStatus osMailFree(osMailQId queue_id, void *mail)
{
    if (queue_id == NULL)
    {
        return osErrorParameter;
    }

    return osPoolFree(queue_id->pool, mail);
}
#endif /* Use Mail Queues */
