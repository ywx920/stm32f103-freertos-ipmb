/**
 * @file shell_task.c
 * @brief Shell 处理任务
 * @details 从队列接收串口数据，在任务上下文中调用 shellHandler 处理命令
 */

#include "shell_task.h"
#include "shell.h"
#include "shell_port.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define SHELL_TASK_PRIO     2
#define SHELL_TASK_STACK    256

static TaskHandle_t xShellTaskHandle = NULL;

/**
 * @brief Shell 处理任务
 * @param pvParameters 任务参数（未使用）
 * @retval 无
 */
static void vShellTask(void *pvParameters)
{
    uint8_t ch;
    QueueHandle_t xQueue;
    
    (void)pvParameters;
    
    // 获取队列句柄
    xQueue = xGetShellQueue();
    if (xQueue == NULL)
    {
        // 队列创建失败，任务退出
        vTaskDelete(NULL);
        return;
    }
    
    for (;;)
    {
        // 从队列等待接收数据
        if (xQueueReceive(xQueue, &ch, portMAX_DELAY) == pdTRUE)
        {
            // 在任务上下文中调用 shellHandler（安全执行 shell 命令）
            shellHandler(&shell, ch);
        }
    }
}

/**
 * @brief 创建 Shell 处理任务
 * @param 无
 * @retval pdTRUE: 成功, pdFALSE: 失败
 */
BaseType_t xShellTaskCreate(void)
{
    BaseType_t xResult;
    
    // 先创建队列
    if (xShellQueueCreate() == NULL)
    {
        return pdFALSE;
    }
    
    // 创建 Shell 处理任务
    xResult = xTaskCreate(
        vShellTask,
        "ShellTask",
        SHELL_TASK_STACK,
        NULL,
        SHELL_TASK_PRIO,
        &xShellTaskHandle
    );
    
    return xResult;
}
