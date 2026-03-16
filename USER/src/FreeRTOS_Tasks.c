#include "FreeRTOS_Tasks.h"
#include "shell_task.h"
#include "led.h"
#include "bsp_reset.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include <stdio.h>

/******************************************************************************************************/
/*FreeRTOS任务配置*/

/* 启动任务配置 - 用于创建其他任务 */
#define START_TASK_PRIO     1
#define START_STK_SIZE      128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

/* LED 任务配置 */
#define LED_TASK_PRIO       3
#define LED_STK_SIZE        128
TaskHandle_t LEDTask_Handler;
void vLedTask(void *pvParameters);

/* 电源控制任务配置（预留，用于后续扩展） */
#define POWER_TASK_PRIO     4
#define POWER_STK_SIZE      256
TaskHandle_t PowerTask_Handler;
void vPowerTask(void *pvParameters);

/**
 * @brief 创建所有 FreeRTOS 任务
 * @param 无
 * @retval 无
 */
void os_task(void)
{
    xTaskCreate((TaskFunction_t)start_task,
                (const char *)"start_task",
                (uint16_t)START_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIO,
                (TaskHandle_t *)&StartTask_Handler);
    
    vTaskStartScheduler();
}

/**
 * @brief 打印版本信息
 */
static void Printf_Version(void)
{
    printf("\r\nProject:%s %s\r\n", __DATE__, __TIME__);
}

/**
 * @brief 启动任务 - 执行初始化并创建其他任务
 * @param pvParameters 传入参数（未用到）
 * @retval 无
 */
void start_task(void *pvParameters)
{
    (void)pvParameters;
    
    /* 执行电源复位时序（现在可以使用 vTaskDelay） */
    powerReset();
    Printf_Version();
    
    Usart_SendString(DEBUG_USARTx, "\r\n串口中断接收缓冲区测试\r\n");
    printf("\r\nSTM32F103 USART1 printf OK 1 \r\n");
    Usart_SendString(DEBUG_USART3, "\r\n串口中断接收测试\r\n");
    uart3_printf("USART3 串口打印测试\r\n");
    
    taskENTER_CRITICAL(); /* 进入临界区 */
    
    /* 创建 LED 任务 */
    xTaskCreate((TaskFunction_t)vLedTask,
                (const char *)"LedTask",
                (uint16_t)LED_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED_TASK_PRIO,
                (TaskHandle_t *)&LEDTask_Handler);
    
    /* 创建 Shell 处理任务 */
    if (xShellTaskCreate() != pdTRUE)
    {
        // Shell 任务创建失败，可以在这里处理错误
    }
    
    vTaskDelete(StartTask_Handler); /* 删除启动任务 */
    taskEXIT_CRITICAL();             /* 退出临界区 */
}

/**
 * @brief LED 任务 - 运行 LED 闪烁
 * @param pvParameters 传入参数（未用到）
 * @retval 无
 */
void vLedTask(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;)
    {
        LED_RUN();  // 使用 vTaskDelay 实现延时
    }
}

/**
 * @brief 电源控制任务（示例，用于后续扩展）
 * @param pvParameters 传入参数（未用到）
 * @retval 无
 * @note  可以通过命令队列接收 power_on/power_off/reset 等命令
 */
void vPowerTask(void *pvParameters)
{
    (void)pvParameters;
    
    for (;;)
    {
        // 预留：通过队列接收命令并执行电源控制
        // 避免在 shell 中断中直接执行耗时的电源操作
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
