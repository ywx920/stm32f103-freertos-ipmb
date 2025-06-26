#ifndef __BSP_RESET_H
#define __BSP_RESET_H

#include "stm32f10x.h"
#include "SysTick.h"
#include <stdio.h>
#include "stm32f10x_gpio.h"

/* 复位相关信号 */
#define RCC_RST_A RCC_APB2Periph_GPIOA
#define RCC_RST_B RCC_APB2Periph_GPIOB
#define RCC_RST_C RCC_APB2Periph_GPIOC
#define RCC_RST_D RCC_APB2Periph_GPIOD

// #define GPIO_PORT_A				    GPIOA
// #define GPIO_PORT_B				    GPIOB
// #define GPIO_PORT_C				    GPIOC
// #define GPIO_PORT_D				    GPIOD

/* PB0  默认低电平 DSP_A 与 DSP_B 的上电复位信号 */
#define DSP_POR_MCU GPIO_Pin_0

/* ------------------------- DSP_A ------------------------- */

/* PA7  0：复位进行中，1：复位完成  DSP_A 复位指示信号 */
#define A_RST_STAT_MCU GPIO_Pin_7
/* PA11 默认低电平  DSP_A 外部PCIE复位信号 */
#define A_PCIE_RSTN GPIO_Pin_11
/* PB1  默认低电平  DSP_A 全局复位信号 */
#define A_DSP_RSTN_MCU GPIO_Pin_1
/* PA15 默认低电平  PHY_A 复位信号 */
#define A_PHY_RSTN GPIO_Pin_15
/* PB4  默认高电平  DSP_A 的PCIE使能信号 1：使能，0：禁止 */
#define A_DSP_PCIESSEN_MCU GPIO_Pin_4

/* ------------------------- DSP_B ------------------------- */

/* PA8  0：复位进行中，1：复位完成  DSPB复位指示信号 */
#define B_RST_STAT_MCU GPIO_Pin_8
/* PA12 默认低电平  DSPB外部PCIE复位信号 */
#define B_PCIE_RSTN GPIO_Pin_12
/* PB3  默认低电平  DSPB 全局复位信号 */
#define B_DSP_RSTN_MCU GPIO_Pin_3
/* PC2  默认低电平  PHY_B 复位信号 */
#define B_PHY_RSTN GPIO_Pin_2
/* PB5  默认高电平  DSP_A 的PCIE使能信号 1：使能，0：禁止 */
#define B_DSP_PCIESSEN_MCU GPIO_Pin_5

/* -----------------电源使能信号 低电平--------------------- */

/* PB8  */
#define P1V8_EN GPIO_Pin_8
/* PB12 */
#define VCORE_VDDQ_EN GPIO_Pin_12
/* PB14 */
#define VDDP_VDDP15_EN GPIO_Pin_14
/* PC3  */
#define P2V5_PHY_EN GPIO_Pin_3
/* PC5  低电平  */
#define P1V2_PHY_EN GPIO_Pin_5
/* PC12 低电平  */
#define P3V3_EN GPIO_Pin_12

/* ----------------------PG检测信号------------------------- */

/* PB9  */
#define P1V8_PG GPIO_Pin_9
/* PB13 */
#define VCORE_VDDQ_PG GPIO_Pin_13
/* PB15 */
#define VDDP_VDDP15_PG GPIO_Pin_15
/* PC4  */
#define P2V5_PHY_PG GPIO_Pin_4
/* PC8  */
#define P1V2_PHY_PG GPIO_Pin_8
/* PD2  */
#define P3V3_PG GPIO_Pin_2

void ResetInit(void);
void powerReset(void);
void powerOn(void);
void powerOff(void);
void soft_reset(void);

#endif
