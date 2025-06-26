/********************       (C) COPYRIGHT 2017  ***************************
 * 文件名  ：led.c
 * 描述    ：led 应用函数库
 * 实验平台：
 * 硬件连接：-----------------------
 *          |   PB12 	- LED1		   |
 *          |   PB13 	- LED2		   |
 *           -----------------------
 * 库版本  ：ST3.5.0
 * 编写日期：2015-03-30
 * 修改日期：2017-03-24
 * 作者    ：
 ****************************************************************************/
#include "led.h"

/*
 * 函数名：LED_GPIO_Config
 * 描述  ：配置LED_RUN用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void LED_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启 LED1 的GPIOx的外设时钟*/
	RCC_APB2PeriphClockCmd(RCC_LED1, ENABLE);
	/*选择要控制的LED1 GPIOx引脚*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LED1;
	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/*设置引脚速率为50MHz */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*调用库函数，初始化LED1 的GPIOx*/
	GPIO_Init(GPIO_LED1_PORT, &GPIO_InitStructure);

	// /*开启 LED2 的GPIOx的外设时钟*/
	// RCC_APB2PeriphClockCmd(RCC_LED2, ENABLE);
	// /*选择要控制的LED2 GPIOx引脚*/
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LED2;
	// /*设置引脚模式为通用推挽输出*/
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// /*设置引脚速率为50MHz */
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// /*调用库函数，初始化LED2 的GPIOx*/
	// GPIO_Init(GPIO_LED2_PORT, &GPIO_InitStructure);

	/* 关闭LED1,LED2	*/
	LED1(ON);
	// LED2(OFF);
}

void LED_RUN(void)
{
	LED1(ON);
	// LED2(ON);
	delay_ms(500);
	LED1(OFF);
	delay_ms(500);

	// vTaskDelay(1000);
}

/******************* (C) COPYRIGHT  2017 *****END OF FILE************/
