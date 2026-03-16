#include "bsp_i2c_slave.h"
#include "ipmb_protocol.h"
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stdio.h>
#include "shell.h"
#include "shell_port.h"
#include "usart.h"
#include "SysTick.h"

#define RX_BUF_LEN 32
#define TX_BUF_LEN 32

static uint8_t i2c1_rx_buf[RX_BUF_LEN] = {0};
static uint8_t i2c1_tx_buf[TX_BUF_LEN] = {0};
static uint8_t i2c2_rx_buf[RX_BUF_LEN] = {0};
static uint8_t i2c2_tx_buf[TX_BUF_LEN] = {0};

// Clear ADDR by reading SR1, then SR2
static void I2C_clear_ADDR(I2C_TypeDef *I2Cx)
{
    (void)I2Cx->SR1;
    (void)I2Cx->SR2;
}

// Clear STOPF by reading SR1, then writing CR1
static void I2C_clear_STOPF(I2C_TypeDef *I2Cx)
{
    (void)I2Cx->SR1;
    I2C_Cmd(I2Cx, ENABLE);
}

// 清除所有 I2C 错误标志
static void I2C_ClearAllErrorFlags(I2C_TypeDef *I2Cx)
{
    // 读取 SR1 清除所有错误标志
    (void)I2Cx->SR1;
    
    // 软件清除错误标志
    I2C_ClearITPendingBit(I2Cx, I2C_IT_SMBALERT);
    I2C_ClearITPendingBit(I2Cx, I2C_IT_TIMEOUT);
    I2C_ClearITPendingBit(I2Cx, I2C_IT_PECERR);
    I2C_ClearITPendingBit(I2Cx, I2C_IT_OVR);
    I2C_ClearITPendingBit(I2Cx, I2C_IT_AF);
    I2C_ClearITPendingBit(I2Cx, I2C_IT_ARLO);
    I2C_ClearITPendingBit(I2Cx, I2C_IT_BERR);
}

void I2C_Slave_Init(I2C_TypeDef *I2Cx, uint8_t address)
{
    printf("I2C_Slave_Init: I2Cx=0x%p, address=0x%02X\r\n", (void*)I2Cx, address);
    
    I2C_InitTypeDef i2c;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;

    if (I2Cx == I2C1)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    else if (I2Cx == I2C2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Pin = ((I2Cx == I2C1) ? (GPIO_Pin_6 | GPIO_Pin_7) : (GPIO_Pin_10 | GPIO_Pin_11));
    GPIO_Init(GPIOB, &gpio);

    I2C_Cmd(I2Cx, DISABLE);
    I2C_DeInit(I2Cx);
    
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = address << 1;  // 地址左移1位（7位地址模式）
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c.I2C_ClockSpeed = 100000;
    I2C_Init(I2Cx, &i2c);

    I2C_ITConfig(I2Cx, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
    I2C_ITConfig(I2Cx, I2C_IT_ERR, ENABLE);
    I2C_Cmd(I2Cx, ENABLE);

    // 配置事件中断
    nvic.NVIC_IRQChannel = (I2Cx == I2C1) ? I2C1_EV_IRQn : I2C2_EV_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    // 配置错误中断
    nvic.NVIC_IRQChannel = (I2Cx == I2C1) ? I2C1_ER_IRQn : I2C2_ER_IRQn;
    NVIC_Init(&nvic);
}

// I2C1 事件中断处理
void I2C1_EV_IRQHandler(void)
{
    static uint8_t idx = 0;
    uint32_t event;
    
    // 获取最后事件
    event = I2C_GetLastEvent(I2C1);
    
    switch (event)
    {
        // ========== 从机接收 ==========
        case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: // EV1
            I2C_clear_ADDR(I2C1);
            idx = 0;
            break;
            
        case I2C_EVENT_SLAVE_BYTE_RECEIVED: // EV2
            if (idx < RX_BUF_LEN)
            {
                i2c1_rx_buf[idx++] = I2C_ReceiveData(I2C1);
            }
            else
            {
                (void)I2C_ReceiveData(I2C1);  // 丢弃溢出数据
            }
            break;
            
        case I2C_EVENT_SLAVE_STOP_DETECTED: // EV4
            I2C_clear_STOPF(I2C1);
            // 可以在这里处理接收到的数据
            // IPMB_Process_Command(i2c1_rx_buf, i2c1_tx_buf, &len);
            break;
            
        // ========== 从机发送 ==========
        case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: // EV1
            I2C_clear_ADDR(I2C1);
            idx = 0;
            I2C_SendData(I2C1, i2c1_tx_buf[idx++]);
            break;
            
        case I2C_EVENT_SLAVE_BYTE_TRANSMITTED: // EV3
            if (idx < TX_BUF_LEN)
            {
                I2C_SendData(I2C1, i2c1_tx_buf[idx++]);
            }
            else
            {
                I2C_SendData(I2C1, 0x00);  // 发送空字节
            }
            break;
            
        case I2C_EVENT_SLAVE_ACK_FAILURE: // EV3-2
            I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
            break;
            
        // ========== 其他事件（暂不处理但清除）==========
        case I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED:
        case I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED:
        case I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED:
            I2C_clear_ADDR(I2C1);
            break;
            
        // ========== 主机事件（本设备作为从机，不处理）==========
        case I2C_EVENT_MASTER_MODE_SELECT:
        case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
        case I2C_EVENT_MASTER_BYTE_RECEIVED:
        case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
        case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
        case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
        case I2C_EVENT_MASTER_MODE_ADDRESS10:
            // 不应该进入主机模式，读取 SR 清除标志
            (void)I2C1->SR1;
            (void)I2C1->SR2;
            break;
            
        default:
            // 未知事件，清除所有标志
            (void)I2C1->SR1;
            (void)I2C1->SR2;
            break;
    }
}

// I2C1 错误中断处理
void I2C1_ER_IRQHandler(void)
{
    // 清除所有错误标志
    I2C_ClearAllErrorFlags(I2C1);
    
    // 可以在这里添加错误计数或日志
    // printf("I2C1 Error\r\n");
}

// I2C2 事件中断处理
void I2C2_EV_IRQHandler(void)
{
    static uint8_t idx = 0;
    uint32_t event;
    
    event = I2C_GetLastEvent(I2C2);
    
    switch (event)
    {
        case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
            I2C_clear_ADDR(I2C2);
            idx = 0;
            break;
            
        case I2C_EVENT_SLAVE_BYTE_RECEIVED:
            if (idx < RX_BUF_LEN)
            {
                i2c2_rx_buf[idx++] = I2C_ReceiveData(I2C2);
            }
            else
            {
                (void)I2C_ReceiveData(I2C2);
            }
            break;
            
        case I2C_EVENT_SLAVE_STOP_DETECTED:
            I2C_clear_STOPF(I2C2);
            {
                uint8_t len = 0;
                IPMB_Process_Command(i2c2_rx_buf, i2c2_tx_buf, &len);
            }
            break;
            
        case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
            I2C_clear_ADDR(I2C2);
            idx = 0;
            I2C_SendData(I2C2, i2c2_tx_buf[idx++]);
            break;
            
        case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:
            if (idx < TX_BUF_LEN)
            {
                I2C_SendData(I2C2, i2c2_tx_buf[idx++]);
            }
            else
            {
                I2C_SendData(I2C2, 0x00);
            }
            break;
            
        case I2C_EVENT_SLAVE_ACK_FAILURE:
            I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
            break;
            
        default:
            (void)I2C2->SR1;
            (void)I2C2->SR2;
            break;
    }
}

// I2C2 错误中断处理
void I2C2_ER_IRQHandler(void)
{
    I2C_ClearAllErrorFlags(I2C2);
}

// 调试命令：测试 I2C
int ipmb_task(void)
{
    printf("I2C1 RX: ");
    for (int i = 0; i < RX_BUF_LEN; i++)
    {
        printf("%02X ", i2c1_rx_buf[i]);
    }
    printf("\r\nI2C1 TX: ");
    for (int a = 0; a < 10; a++)
    {
        printf("%02X ", i2c1_tx_buf[a]);
    }
    printf("\r\n");
    
    delay_us(1000);
    return 0;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    i2ctest, ipmb_task, i2ctest);
