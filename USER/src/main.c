/********************        (C) COPYRIGHT 2017        **************************
 * 文件名  ：main.c
 * 描述    ：基于FreeRTOS的LED的测试程序
 * 实验平台：STM32F103RBT6
 * 库版本  ：ST3.5.0
 * 编写日期：2015-03-30
 * 修改日期：2017-03-24
 * 作者    ：
 * 2017-03-24
 * (1)运行LED测试正常
 * 2017-03-24
 *（1）加入FreeRTOS的支持
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

void Printf_Version(void)
{
	printf("\r\nProject:%s%s%s%s %s\r\n", __PROJECT, __VERSION, __SOFT_NUM, __DATE__, __TIME__);
}

void System_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	SysTick_Init();

	USART1_Config();
	USART3_Config();

	LED_GPIO_Config();
	ADCx_Init();
	ResetInit();
	userShellInit();

	LT2991_Init();
	GPIO_Key_Init();
	I2C_Slave_Init(I2C1, 0xA0);
}

/*
 * 函数名：main
 * 描述  : "主机"的主函数
 * 输入  ：无
 * 输出  : 无
 */

int main(void)
{
	System_Init();
	// os_task();
	
	powerReset();
	Printf_Version();


	Usart_SendString(DEBUG_USARTx, "\r\n串口中断接收回显测试\r\n");
	printf("\r\nSTM32F103 USART1 printf OK 1 \r\n");
	Usart_SendString(DEBUG_USART3, "\r\n串口中断接收回显\r\n");
	uart3_printf("USART3 串口打印测试\r\n");

	while (1)
	{
		LED_RUN();
		//LT2991_ReadAll();
		//printf("\r\n STM32F103 USART1 OK 2 \r\n");
		//uart3_printf("USART3 TEST OK 2 \r\n");
		//Usart_SendString(DEBUG_USART3, "\r\n串口中断接收回显\r\n");

		// ADC_Read();
		// GPIO_Read_Keys();
		//delay_ms(1000);
	}
}
