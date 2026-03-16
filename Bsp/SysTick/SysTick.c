
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief  毫秒延时函数（使用 FreeRTOS vTaskDelay）
 * @param  ms: 延时毫秒数
 * @retval 无
 * @note   此函数必须在任务上下文中调用，不能在中断中使用
 */
void delay_ms(uint32_t ms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        // FreeRTOS 已启动，使用 vTaskDelay
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
    else
    {
        // FreeRTOS 未启动，使用空循环粗略延时
        // 72MHz 下，约 72000 个周期为 1ms
        volatile uint32_t i, j;
        for (i = 0; i < ms; i++)
            for (j = 0; j < 7200; j++);
    }
}

/**
 * @brief  微秒延时函数（空循环实现）
 * @param  us: 延时微秒数
 * @retval 无
 * @note   此函数可在中断或任务中使用，精度约 10us
 */
void delay_us(uint32_t us)
{
    // 72MHz 下，约 72 个周期为 1us
    // 加上函数调用开销，实际精度约 10us
    volatile uint32_t i;
    for (i = 0; i < us * 7; i++);
}

/*********************************************END OF FILE**********************/
