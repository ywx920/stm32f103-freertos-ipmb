/********************        (C) COPYRIGHT 2017        **************************
 * 文件  ：main.c
 * 描述  ：基于 FreeRTOS 的 LED 控制与 Shell 终端程序
 * 平台  ：STM32F103RBT6
 * 版本  ：ST3.5.0
 * 日期  ：2024
 **********************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "led.h"
#include "shell.h"
#include "shell_port.h"
#include "usart.h"
#include "lt2991.h"
#include "bsp_i2c_slave.h"
#include "bsp_gpio.h"
#include "SysTick.h"
#include "bsp_adc.h"
#include "FreeRTOS_Tasks.h"
#include "bsp_reset.h"

float voltages[7]; // V1~V5, V7, V8

/**
 * @brief 系统初始化
 * @param 无
 * @retval 无
 */
void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    // 注意：SysTick 由 FreeRTOS 接管，不需要单独初始化
    
    USART1_Config();
    USART3_Config();

    LED_GPIO_Config();
    ADCx_Init();
    ResetInit();
    userShellInit();

    LT2991_Init();
    GPIO_Key_Init();
    I2C_Slave_Init(I2C1, 0xA0);  // 第一路 I2C，地址 0xA0
    I2C_Slave_Init(I2C2, 0xA2);  // 第二路 I2C，地址 0xA2（或其他地址）
}

/**
 * @brief 主函数
 * @param 无
 * @retval 无
 */
int main(void)
{
    System_Init();
    
    /* 
     * 创建并启动 FreeRTOS 任务
     * 此函数不会返回，由调度器接管
     */
    os_task();
    
    /* 永远不会执行到这里 */
    while (1);
}
