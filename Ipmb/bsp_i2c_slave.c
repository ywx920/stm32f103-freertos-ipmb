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

#define RX_BUF_LEN 32
#define TX_BUF_LEN 32

static uint8_t i2c1_rx_buf[RX_BUF_LEN] = {0};
static uint8_t i2c1_tx_buf[TX_BUF_LEN] = {0};
static uint8_t i2c2_rx_buf[RX_BUF_LEN] = {0};
static uint8_t i2c2_tx_buf[TX_BUF_LEN] = {0};

void I2C_Slave_Init(I2C_TypeDef *I2Cx, uint8_t address)
{
	printf("I2C_Slave_Init\r\n");
	I2C_InitTypeDef i2c;
	GPIO_InitTypeDef gpio;
	NVIC_InitTypeDef nvic;

	if (I2Cx == I2C1)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	if (I2Cx == I2C2)
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
	i2c.I2C_OwnAddress1 = address;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c.I2C_ClockSpeed = 100000;
	I2C_Init(I2Cx, &i2c);

	I2C_ITConfig(I2Cx, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
	I2C_ITConfig(I2Cx, I2C_IT_ERR, ENABLE);
	I2C_Cmd(I2Cx, ENABLE);

	if (I2Cx == I2C1)
	{
		nvic.NVIC_IRQChannel = I2C1_EV_IRQn;
	}
	else
	{
		nvic.NVIC_IRQChannel = I2C2_EV_IRQn;
	}
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

// void I2C1_EV_IRQHandler(void) {
//     static uint8_t idx = 0;
//     if (I2C_CheckEvent(I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED)) {
//         idx = 0;
//     } else if (I2C_CheckEvent(I2C1, I2C_EVENT_SLAVE_BYTE_RECEIVED)) {
//         i2c1_rx_buf[idx++] = I2C_ReceiveData(I2C1);
//     } else if (I2C_CheckEvent(I2C1, I2C_EVENT_SLAVE_STOP_DETECTED)) {
//         uint8_t len = 0;
//         IPMB_Process_Command(i2c1_rx_buf, i2c1_tx_buf, &len);
//         idx = 0;
//         I2C_ClearFlag(I2C1, I2C_FLAG_STOPF);
//     } else if (I2C_CheckEvent(I2C1, I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) ||
//                I2C_CheckEvent(I2C1, I2C_EVENT_SLAVE_BYTE_TRANSMITTING)) {
//         I2C_SendData(I2C1, i2c1_tx_buf[idx++]);
//     }
// }

// Clear ADDR by reading SR1, then SR2
void I2C_clear_ADDR(I2C_TypeDef *I2Cx)
{
	I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR);
	((void)(I2Cx->SR2));
}

// Clear STOPF by reading SR1, then writing CR1
void I2C_clear_STOPF(I2C_TypeDef *I2Cx)
{
	I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF);
	I2C_Cmd(I2Cx, ENABLE);
}

static uint8_t data = 0x66;
static uint16_t recdata = 0;


void I2C1_EV_IRQHandler(void)
{
	uint8_t idx = 0;
	// Clear AF from slave-transmission end
	if (I2C_GetITStatus(I2C1, I2C_IT_AF))
	{
		I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
	}
	// Big state machine response, since doesn't actually keep state
	switch (I2C_GetLastEvent(I2C1))
	{
	// SLAVE
	// Receive
	case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: // EV1 从设备接收时的地址匹配
	{
		I2C_clear_ADDR(I2C1);
		idx = 0;
		break;
	}
	case I2C_EVENT_SLAVE_BYTE_RECEIVED: // EV2 从设备接收数据时的字节接收
	{									// Read it, so no one is waiting, clears BTF if necessary
		recdata = I2C_ReceiveData(I2C1);
		Usart_SendByte1(USART1, recdata);
		// i2c1_rx_buf[idx++] = I2C_ReceiveData(I2C1);
		// Usart_SendArray(USART1, i2c1_rx_buf, idx);
		// uint8_t len = 0;
		// IPMB_Process_Command(i2c1_rx_buf, i2c1_tx_buf, &len);
		// data = I2C_ReceiveData(I2C1);


		// LED1= !(data & 0x01);
		// LED2= !(data & 0x02);
		// LED3= !(data & 0x04);
		// LED4= !(data & 0x08);

		// Do something with it
		if (I2C_GetFlagStatus(I2C1, I2C_FLAG_DUALF)) // 检查是否为次级地址接收
		{											 // Secondary Receive
		}
		else if (I2C_GetFlagStatus(I2C1, I2C_FLAG_GENCALL))
		{ // General Receive
		}
		else
		{ // Normal
		}
		break;
	}
	case I2C_EVENT_SLAVE_STOP_DETECTED: // End of receive, EV4 从设备接收数据时的停止检测

	{
		I2C_clear_STOPF(I2C1);
		break;
	}

	// Transmit
	case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: // EV1  从设备发送数据时的地址匹配
	{
		I2C_clear_ADDR(I2C1);
		// Send first byte
		//  I2C_SendData(I2C1, i2c1_tx_buf[0]);
		I2C_SendData(I2C1, data);
		// printf("%d\r\n", data);
		break;
	}
	case I2C_EVENT_SLAVE_BYTE_TRANSMITTED: // EV3  从设备发送数据时的字节发送
	{									   // Determine what you want to send
		// data = 5;
		if (I2C_GetFlagStatus(I2C1, I2C_FLAG_DUALF))
		{ // Secondary Transmit
		}
		else if (I2C_GetFlagStatus(I2C1, I2C_FLAG_GENCALL))
		{ // General Transmit
		}
		else
		{ // Normal
		}
		// Read flag and write next byte to clear BTF if present
		I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF);
		// I2C_SendData(I2C1, i2c1_tx_buf[0]);
		I2C_SendData(I2C1, data);
		// printf("%d\r\n", data);

		break;
	}
	case I2C_EVENT_SLAVE_ACK_FAILURE: // End of transmission EV3_2  从设备发送数据时的ACK失败
	{								  // TODO: Doesn't seem to be getting reached, so just
		// check at top-level
		I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
		break;
	}
	// Alternative Cases for address match 地址匹配的其他情况
	case I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: // EV1
	{
		break;
	}
	case I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: // EV1
	{
		break;
	}
	case I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: // EV1
	{
		break;
	}
	// MASTER  主设备事件
	case I2C_EVENT_MASTER_MODE_SELECT: // EV5, just sent start bit
	{
		break;
	} // Receive
	case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: // EV6, just sent addr
	{
		break;
	}
	case I2C_EVENT_MASTER_BYTE_RECEIVED: // EV7
	{
		break;
	} // Transmit
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: // EV6, just sent addr
	{
		break;
	}
	case I2C_EVENT_MASTER_BYTE_TRANSMITTING: // EV8, about to send data
	{
		break;
	}
	case I2C_EVENT_MASTER_BYTE_TRANSMITTED: // EV8_2, just sent data
	{
		break;
	}
	// Alternative addressing stuff, not going to worry about
	case I2C_EVENT_MASTER_MODE_ADDRESS10: // EV9
	{
		break;
	}
	default:
		// How the FUCK did you get here?
		// I should probably raise some error, but fuck it,
		// it's late
		{
			break;
		}
	}
}

void I2C1_ER_IRQHandler(void)
{
	//       GPIO_SetBits(GPIOD, RED);
	//	LED3=0;
	// Can't use nice switch statement, because no fxn available
	if (I2C_GetITStatus(I2C1, I2C_IT_SMBALERT))
	{
	}
	else if (I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT))
	{
	}
	else if (I2C_GetITStatus(I2C1, I2C_IT_PECERR))
	{
	}
	else if (I2C_GetITStatus(I2C1, I2C_IT_OVR))
	{
		// Overrun
		// CLK stretch disabled and receiving
		// DR has not been read, b4 next byte comes in
		// effect: lose byte
		// should:clear RxNE and transmitter should retransmit

		// Underrun
		// CLK stretch disabled and I2C transmitting
		// haven't updated DR since new clock
		// effect: same byte resent
		// should: make sure discarded, and write next
	}
	else if (I2C_GetITStatus(I2C1, I2C_IT_AF))
	{
		// Detected NACK
		// Transmitter must reset com
		// Slave: lines released
		// Master: Stop or repeated Start must must be generated
		// Master = MSL bit
		// Fixup
		I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
	}
	else if (I2C_GetITStatus(I2C1, I2C_IT_ARLO))
	{
		// Arbitration Lost
		// Goes to slave mode, but can't ack slave address in same transfer
		// Can after repeat Start though
	}
	else if (I2C_GetITStatus(I2C1, I2C_IT_BERR))
	{
		// Bus Error
		// In slave mode: data discarded, lines released, acts like restart
		// In master mode: current transmission continues
	}
}

void I2C2_EV_IRQHandler(void)
{
	static uint8_t idx = 0;
	if (I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED))
	{
		idx = 0;
	}
	else if (I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_BYTE_RECEIVED))
	{
		i2c2_rx_buf[idx++] = I2C_ReceiveData(I2C2);
	}
	else if (I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_STOP_DETECTED))
	{
		uint8_t len = 0;
		IPMB_Process_Command(i2c2_rx_buf, i2c2_tx_buf, &len);
		idx = 0;
		I2C_ClearFlag(I2C2, I2C_FLAG_STOPF);
	}
	else if (I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) ||
			 I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_BYTE_TRANSMITTING))
	{
		I2C_SendData(I2C2, i2c2_tx_buf[idx++]);
	}
}

int ipmb_task(void)
{
	GPIO_Key_Init();
	// uint8_t i2c_address = GPIO_Read_Keys(); // 读取按键状态
	// uint8_t i2c_address = 0xa0; // 读取按键状态
	// I2C_Slave_Init(I2C1, i2c_address);
	// I2C_Slave_Init(I2C2, i2c_address | 0x02); // I2C2 地址加1

	// while (1)
	// {
		// 所有处理在中断里
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
		I2C_SendData(I2C1, 0x55);
		data++;
		printf("data:%02X\r\n", data);
		delay_1us(1000);
		// vTaskDelay(2000);
	// }
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
	i2ctest, ipmb_task, ipmb_task);
