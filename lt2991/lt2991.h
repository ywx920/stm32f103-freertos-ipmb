#ifndef __LT2991_H
#define __LT2991_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "soft_i2c.h"

#define LT2991_ADDR    0x98
#define Vol_SINGLE_LSB    (305.18) //单端电压采集分辨率 305.18uV = 2.5/(2^13)
#define Vol_DIFFERENTIAL_LSB    (19.075) //差分电压采集分辨率 19.075uV = 2.5/(2^17)
#define VCC_LSB    (305.18) //VCC采集分辨率 305.18uV = 2.5/(2^13)
#define VCC_BASE    (2500)   //VCC=result+2.5V
#define T_LSB    (0.0625)  //温度的分辨率
#define DIODE_LSB   (38.15)   //二极管电压采集分辨率为38.15uV 
#define XZC2991Adr  0x98  // ADR2=1,ADR1=0,ADR0=0 

#define LT2991_V1V2SWITCH  1
typedef enum
{
	REG_STA_LOW = 0X00,
	REG_STA_HIGH,
	REG_RSV1,
	REG_RSV2,
	REG_RSV3,
	REG_RSV4,
	REG_CTRL_V1_4,
	REG_CTRL_V5_8,
	REG_PWM_LSB,
	REG_PWM_MSB,
	REG_V1_MSB,
	REG_V1_LSB,
	REG_V2_MSB,
	REG_V2_LSB,
	REG_V3_MSB,
	REG_V3_LSB,
	REG_V4_MSB,
	REG_V4_LSB,
	REG_V5_MSB,
	REG_V5_LSB,
	REG_V6_MSB,
	REG_V6_LSB,
	REG_V7_MSB,
	REG_V7_LSB,
	REG_V8_MSB,
	REG_V8_LSB,
	REG_T_INT_MSB,
	REG_T_INT_LSB,
	REG_VCC_MSB,
	REG_VCC_LSB,
}Ltc2991_Reg;



void LT2991_Init(void);

#define LT2991_CHANNEL_COUNT 8

// void LT2991_ReadAll();
float LT2991_ReadVoltage(uint8_t reg);
void LT2991_Read_V3V4V5V6(float *v3, float *v4, float *v5, float *v6);
float LT2991_ReadTemperature(uint8_t reg);

#endif
