/*
****************************************************************
    @Copyright (C),
    @File name:           		USART.c
    @Author:
    @Version:             		V1.0
    @Date:  					2023-09-27
    @Description:
    @Others:
    @attention:

    @History:

***************************************************************
*/

#include "usart.h"
#include "shell_port.h"
#include "queue.h"

/* Shell 输入数据队列句柄 */
static QueueHandle_t xShellQueue = NULL;

/**
 * @brief  创建 Shell 输入队列
 * @param  无
 * @retval 队列句柄
 */
QueueHandle_t xShellQueueCreate(void)
{
    xShellQueue = xQueueCreate(16, sizeof(uint8_t));
    return xShellQueue;
}

/**
 * @brief  获取 Shell 输入队列句柄
 * @param  无
 * @retval 队列句柄
 */
QueueHandle_t xGetShellQueue(void)
{
    return xShellQueue;
}

// 配置嵌套向量中断控制器NVIC
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                 //嵌套向量中断控制器组选择
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;     // 配置USART为中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
    NVIC_Init(&NVIC_InitStructure);                           // 初始化配置NVIC
}

// USART GPIO 配置,工作参数配置
void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE); // 打开串口GPIO的时钟
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);           // 打开串口外设的时钟

    // 将USART Tx的GPIO配置为推挽复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // 将USART Rx的GPIO配置为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // 配置串口的工作参数
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;                      // 配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 配置 针数据字长
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 配置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 配置校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 配置硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 配置工作模式，收发一起
    USART_Init(DEBUG_USARTx, &USART_InitStructure);                                 // 完成串口的初始化配置

    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE); // 使能串口接收中断
    NVIC_EnableIRQ(DEBUG_USART_IRQ);                     // 使能USART中断
    USART_Cmd(DEBUG_USARTx, ENABLE);                     // 使能串口
    NVIC_Configuration();                                // 串口中断优先级配置
}

static void NVIC_USART3_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 嵌套向量中断控制器组选择 */
    /* 提示 NVIC_PriorityGroupConfig() 在整个工程只需要调用一次来配置优先级分组*/
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 配置USART为中断源 */
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART3_IRQ;
    /* 抢断优先级*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    /* 子优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* 使能中断 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* 初始化配置NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

void USART3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 打开串口GPIO的时钟
    DEBUG_USART3_GPIO_APBxClkCmd(DEBUG_USART3_GPIO_CLK, ENABLE);

    // 打开串口外设的时钟
    DEBUG_USART3_APBxClkCmd(DEBUG_USART3_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   // 打开复用时钟
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE); // 重定义 GPIO_PartialRemap_USART3   GPIO_FullRemap_USART3

    // 将USART Tx的GPIO配置为推挽复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART3_TX_GPIO_PORT, &GPIO_InitStructure);

    // 将USART Rx的GPIO配置为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART3_RX_GPIO_PORT, &GPIO_InitStructure);

    // 配置串口的工作参数
    // 配置波特率
    USART_InitStructure.USART_BaudRate = DEBUG_USART3_BAUDRATE;
    // 配置 针数据字长
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // 配置停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // 配置校验位
    USART_InitStructure.USART_Parity = USART_Parity_No;
    // 配置硬件流控制
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    // 配置工作模式，收发一起
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    // 完成串口的初始化配置
    USART_Init(DEBUG_USART3, &USART_InitStructure);

    // 串口中断优先级配置
    NVIC_USART3_Configuration();

    // 使能串口接收中断
    USART_ITConfig(DEBUG_USART3, USART_IT_RXNE, ENABLE);

    // 使能串口
    USART_Cmd(DEBUG_USART3, ENABLE);
}

void uart3_printf(const char *fmt, ...)
{
    char buffer[128]; // 声明移到开头
    char *p;          // 循环变量声明移到外部
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    for (p = buffer; *p; p++)
    {
        // 使用SR寄存器和USART_FLAG_TXE
        while (!(DEBUG_USART3->SR & USART_FLAG_TXE))
        {
        }
        DEBUG_USART3->DR = *p; // 使用DR寄存器
    }
}

/*****************  发送一个字节 **********************/
void Usart_SendByte(USART_TypeDef *pUSARTx, uint16_t ch)
{
    USART_SendData(pUSARTx, ch); // 发送一个字节数据到USART
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ; // 等待发送数据寄存器为空
}

#define TX_BUFFER_SIZE 128         // 缓冲区长度
uint8_t tx_buffer[TX_BUFFER_SIZE]; // 发送缓冲区
volatile uint8_t tx_index = 0;     // 当前写入位置
volatile uint8_t tx_length = 0;    // 缓冲区中待发送的字节数
/* 非阻塞模式 */
void Usart_SendByte1(USART_TypeDef *pUSARTx, uint8_t byte)
{
    // 1. 检查缓冲区是否已满
    if (tx_length < TX_BUFFER_SIZE)
    {
        tx_buffer[(tx_index + tx_length) % TX_BUFFER_SIZE] = byte;
        tx_length++;

        // 2. 如果当前未发送，启用发送完成中断（TC）
        if (tx_length == 1)
        {
            USART_ITConfig(pUSARTx, USART_IT_TC, ENABLE);
        }
    }
    // pUSARTx->DR = byte; // 写入发送寄存器
    // 不等待TC标志，由主循环或中断处理后续发送
}
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        if (tx_length > 0)
        {
            // 发送下一字节
            USART1->DR = tx_buffer[tx_index];
            tx_index = (tx_index + 1) % TX_BUFFER_SIZE;
            tx_length--;
        }
        else
        {
            // 缓冲区为空，关闭TC中断
            USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        }
    }
    
    uint8_t ch; // 接收中断缓冲
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART1); // ch = USART1->DR;
        
        // 将数据发送到队列，不在中断中处理 shell
        if (xShellQueue != NULL)
        {
            xQueueSendFromISR(xShellQueue, &ch, &xHigherPriorityTaskWoken);
        }
        
        USART_ClearITPendingBit(USART1, USART_FLAG_RXNE); // 清除接收中断标志位
    }
    
    // 上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/****************** 发送8位的数组 ************************/
void Usart_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;

    for (i = 0; i < num; i++)
    {
        Usart_SendByte(pUSARTx, array[i]); // 发送一个字节数据到USART
    }
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
        ; // 等待发送完成
}

/*****************  发送字符串 **********************/
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte(pUSARTx, *(str + k));
        k++;
    } while (*(str + k) != '\0');
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
        ; // 等待发送完成
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    temp_h = (ch & 0XFF00) >> 8;
    temp_l = ch & 0XFF; // 取出低八位

    USART_SendData(pUSARTx, temp_h); // 发送高八位
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ; // 等待发送完成

    USART_SendData(pUSARTx, temp_l); // 发送低八位
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ; // 等待发送完成
}

/// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    USART_SendData(DEBUG_USARTx, (uint8_t)ch); // 发送一个字节数据到串口
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET)
        ; // 等待发送完毕
    return (ch);
}

/// 重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET)
        ; // 等待串口输入数据
    return (int)USART_ReceiveData(DEBUG_USARTx);
}

// 将字节数据转换为十六进制字符串 [[9]]
void bytes_to_hexstr(uint8_t *data, uint16_t len, char *out)
{
    const char hex_chars[] = "0123456789ABCDEF";
    for (int i = 0; i < len; i++)
    {
        out[i * 3] = hex_chars[(data[i] >> 4) & 0x0F];
        out[i * 3 + 1] = hex_chars[data[i] & 0x0F];
        out[i * 3 + 2] = ' '; // 添加空格分隔
    }
    out[len * 3 - 1] = '\0'; // 最后替换为空格
}

/*********************************************END OF FILE**********************/
