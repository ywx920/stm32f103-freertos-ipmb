#ifndef BSP_I2C_SLAVE_H
#define BSP_I2C_SLAVE_H

#include "stm32f10x.h"
#include "bsp_gpio.h"

void I2C_Slave_Init(I2C_TypeDef* I2Cx, uint8_t address);
int ipmb_task(void);

#endif

