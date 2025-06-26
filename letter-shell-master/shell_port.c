/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include <stdlib.h>
#include "stm32f10x.h"
#include "usart.h"

Shell shell;
char shellBuffer[512];

// static SemaphoreHandle_t shellMutex;

/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        USART_SendData(USART1, (uint8_t)data[i]); /* 等待发送完毕 */
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
            ;
    }
    return 0;
}

/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
// short userShellRead(char *data, unsigned short len)
// {
//     //     // return serialReceive(&debugSerial, (uint8_t *)data, len, 0);
//     stRingBuff *pRxDataBuffer = &gUsart1RxDataBuffer;

//     if (pRxDataBuffer->head != pRxDataBuffer->tail)
//     {
//         ReadRingBuffer(pRxDataBuffer, (unsigned char *)data, len);
//         return len;
//     }
//     return 0;
//     //     while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET); //等待串口输入数据
//     //     return (int)USART_ReceiveData(DEBUG_USARTx);
// }

/**
 * @brief 用户shell上锁
 *
 * @param shell shell
 *
 * @return int 0
 */
// int userShellLock(Shell *shell)
// {
//     xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
//     return 0;
// }

/**
 * @brief 用户shell解锁
 *
 * @param shell shell
 *
 * @return int 0
 */
// int userShellUnlock(Shell *shell)
// {
//     xSemaphoreGiveRecursive(shellMutex);
//     return 0;
// }

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    // shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    // shell.read = userShellRead;
    // shell.lock = userShellLock;
    // shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
    // if (xTaskCreate(shellTask, "shell", 256, &shell, 1, NULL) != pdPASS)
    // {
    //     // logError("shell task creat failed");
    // }
}
// CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);

/* 2. 自己实现shell写函数 */
// signed short User_Shell_Write(char *data, unsigned short len)
// {
//     Uart1_Send_Bytes((uint8_t *)data, (uint16_t)len);
//     return len;
// }

/* 3. 自己实现shell读函数 */
// signed short User_Shell_Read(char *data, unsigned short len)
// {
//     stRingBuff *pRxDataBuffer = &gUsart1RxDataBuffer;

//     if (pRxDataBuffer->head != pRxDataBuffer->tail)
//     {
//         ReadRingBuffer(pRxDataBuffer, (unsigned char *)data, len);
//         return len;
//     }
//     return 0;
// }
