/********************       (C) COPYRIGHT 2015  ***************************
 * 文件名  ：led.h
 * 描述    ：led 应用函数库         
 * 实验平台：
 * 硬件连接：-----------------------
 *          |   PB12 	- LED3		   | 
 *          |   PB13 	- LED4		   | 
 *           ----------------------- 
 * 库版本  ：ST3.5.0
 * 编写日期：2015-03-30
 * 修改日期：2017-03-24
 * 作者    ：
****************************************************************************/
#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"
#include "SysTick.h"
#include "soft_i2c.h"


/* the macro definition to trigger the led on or off 
 * 1 - on
 - 0 - off
 */
 
#define 		ON  		1
#define 		OFF 		0


#define RCC_LED1							RCC_APB2Periph_GPIOC
#define GPIO_LED1_PORT				GPIOC
#define GPIO_Pin_LED1					GPIO_Pin_9

// #define RCC_LED2							RCC_APB2Periph_GPIOA
// #define GPIO_LED2_PORT				GPIOA
// #define GPIO_Pin_LED2					GPIO_Pin_7


#define LED1(a)	if (a)	\
					GPIO_ResetBits(GPIO_LED1_PORT,GPIO_Pin_LED1);\
					else		\
					GPIO_SetBits(GPIO_LED1_PORT,GPIO_Pin_LED1)

// #define LED2(a)	if (a)	\
// 					GPIO_ResetBits(GPIO_LED2_PORT,GPIO_Pin_LED2);\
// 					else		\
// 					GPIO_SetBits(GPIO_LED2_PORT,GPIO_Pin_LED2)

					
void LED_GPIO_Config(void);
void LED_RUN(void);


#endif /* __LED_H */
