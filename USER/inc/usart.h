/*
****************************************************************
	@Copyright (C),
	@File name:           		USART.h
	@Author:
	@Version:             		V1.0
	@Date:  					2023-09-27
	@Description:
	@Others:
	@attention:

	@History:

***************************************************************
*/

#ifndef USART_H
#define USART_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include "stm32f10x_usart.h"
#include "queue.h"

/**
 * 串口宏定义，不同的串口挂载的总线和IO不一样，移植时需要修改这几个宏
 * 1-修改总线时钟的宏，uart1挂载到apb2总线，其他uart挂载到apb1总线
 * 2-修改GPIO的宏
 */

// 串口1-USART1
#define DEBUG_USARTx USART1
#define DEBUG_USART_CLK RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE 115200

// USART GPIO 引脚宏定义
#define DEBUG_USART_GPIO_CLK (RCC_APB2Periph_GPIOA)
#define DEBUG_USART_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT GPIOA
#define DEBUG_USART_TX_GPIO_PIN GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT GPIOA
#define DEBUG_USART_RX_GPIO_PIN GPIO_Pin_10

#define DEBUG_USART_IRQ USART1_IRQn
#define DEBUG_USART_IRQHandler USART1_IRQHandler



/* USART3 TEST  */
#define DEBUG_USART3 USART3
#define DEBUG_USART3_CLK RCC_APB1Periph_USART3
#define DEBUG_USART3_APBxClkCmd RCC_APB1PeriphClockCmd
#define DEBUG_USART3_BAUDRATE 115200

// USART GPIO 引脚宏定义
#define DEBUG_USART3_GPIO_CLK (RCC_APB2Periph_GPIOC)
#define DEBUG_USART3_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd

#define DEBUG_USART3_TX_GPIO_PORT GPIOC
#define DEBUG_USART3_TX_GPIO_PIN GPIO_Pin_10
#define DEBUG_USART3_RX_GPIO_PORT GPIOC
#define DEBUG_USART3_RX_GPIO_PIN GPIO_Pin_11

#define DEBUG_USART3_IRQ USART3_IRQn
#define DEBUG_USART3_IRQHandler USART3_IRQHandler

/* UART4 TEST */
// #define DEBUG_USART3 UART4
// #define DEBUG_USART3_CLK RCC_APB1Periph_UART4
// #define DEBUG_USART3_APBxClkCmd RCC_APB1PeriphClockCmd
// #define DEBUG_USART3_BAUDRATE 115200

// // USART GPIO 引脚宏定义
// #define DEBUG_USART3_GPIO_CLK (RCC_APB2Periph_GPIOC)
// #define DEBUG_USART3_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd

// #define DEBUG_USART3_TX_GPIO_PORT GPIOC
// #define DEBUG_USART3_TX_GPIO_PIN GPIO_Pin_10
// #define DEBUG_USART3_RX_GPIO_PORT GPIOC
// #define DEBUG_USART3_RX_GPIO_PIN GPIO_Pin_11

// #define DEBUG_USART3_IRQ UART4_IRQn
// #define DEBUG_USART3_IRQHandler UART4_IRQHandler

void USART1_Config(void);
void USART3_Config(void);
void Usart_SendByte(USART_TypeDef *pUSARTx, uint16_t ch);
void Usart_SendString(USART_TypeDef *pUSARTx, char *str);
void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);
void uart3_printf(const char *fmt, ...);
void bytes_to_hexstr(uint8_t *data, uint16_t len, char *out);
void Usart_SendByte1(USART_TypeDef *pUSARTx, uint8_t byte);

/* Shell 队列相关函数 */
QueueHandle_t xShellQueueCreate(void);
QueueHandle_t xGetShellQueue(void);
#endif
