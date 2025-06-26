#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#define TASK_SCHEDULE_LOCK    vTaskSuspendAll()
#define TASK_SCHEDULE_UNLOCK  xTaskResumeAll()


#define I2C_SCL_GPIO    GPIOC
#define I2C_SCL_PIN     GPIO_Pin_6
#define I2C_SDA_GPIO    GPIOC
#define I2C_SDA_PIN     GPIO_Pin_7

#define IIC_SCL_1()    GPIO_SetBits(I2C_SCL_GPIO, I2C_SCL_PIN) //SCL
#define IIC_SCL_0()    GPIO_ResetBits(I2C_SCL_GPIO, I2C_SCL_PIN) 
#define IIC_SDA_1()    GPIO_SetBits(I2C_SDA_GPIO, I2C_SDA_PIN) //SDA	
#define IIC_SDA_0()    GPIO_ResetBits(I2C_SDA_GPIO, I2C_SDA_PIN)
#define READ_SDA   	   GPIO_ReadInputDataBit(I2C_SDA_GPIO, I2C_SDA_PIN)

static void SDA_OUT(void);
static void SDA_IN(void);
void delay_1us(uint32_t us);
void Soft_I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_WaitACK(void);
void I2C_SendByte(uint8_t byte);

void I2C_WriteByte(uint8_t addr,uint8_t data,uint8_t device_addr);
uint16_t I2C_ReadByte(uint8_t addr,uint8_t device_addr,uint8_t ByteNumToRead);

#endif
