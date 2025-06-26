#include "FreeRTOS_Tasks.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO 5            /* 任务优先级 */
#define TASK1_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
void task1(void *pvParameters); /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO 3            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO 4            /* 任务优先级 */
#define TASK3_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task3Task_Handler; /* 任务句柄 */
void task3(void *pvParameters); /* 任务函数 */

void os_task(void)
{
    xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
                (const char *)"start_task",          /* 任务名称 */
                (uint16_t)START_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */
    vTaskStartScheduler();
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t)task1,               /* 任务函数 */
                (const char *)"task1",               /* 任务名称 */
                (uint16_t)TASK1_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)TASK1_PRIO,             /* 任务优先级 */
                (TaskHandle_t *)&Task1Task_Handler); /* 任务句柄 */
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t)task2,               /* 任务函数 */
                (const char *)"task2",               /* 任务名称 */
                (uint16_t)TASK2_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)TASK2_PRIO,             /* 任务优先级 */
                (TaskHandle_t *)&Task2Task_Handler); /* 任务句柄 */
    /* 创建任务3 */
    xTaskCreate((TaskFunction_t)task3,               /* 任务函数 */
                (const char *)"task3",               /* 任务名称 */
                (uint16_t)TASK3_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传入给任务函数的参数 */
                (UBaseType_t)TASK3_PRIO,             /* 任务优先级 */
                (TaskHandle_t *)&Task3Task_Handler); /* 任务句柄 */
    vTaskDelete(StartTask_Handler);                  /* 删除开始任务 */
    taskEXIT_CRITICAL();                             /* 退出临界区 */
}

/**
 * @brief       task1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
    printf("freertos task 1\r\n");
    while (1)
    {
        powerReset();
    }
}

/**
 * @brief       task2
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
    printf("freertos task 2\r\n");

    while (1)
    {
        // powerOn();
    }
}

/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
    printf("freertos task 3\r\n");

    while (1)
    {
        // powerOff();
    }
}
