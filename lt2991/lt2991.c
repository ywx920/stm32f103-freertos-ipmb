#include "lt2991.h"
#include "core_cm3.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "shell.h"
#include "shell_port.h"

// 读取指定通道电压，单位V
float LT2991_ReadVoltage(uint8_t reg)
{
  uint8_t data = 0;
  uint8_t data1 = 0;
  uint16_t tem = 0;
  float voltage = 0.0;

  data = I2C_ReadByte(reg, XZC2991Adr, 1);
  data1 = I2C_ReadByte(reg + 1, XZC2991Adr, 1);

  tem = ((data & 0x00ff) << 8) | data1;
  if (tem == 0x3fff)
  { // 读取错误，返回0V
    tem = 0;
  }
  tem = tem & 0x3fff;
  voltage = ((float)tem * Vol_SINGLE_LSB) / 1000;
  return voltage / 1000; // 单位 V
}

void LT2991_Init(void)
{
  Soft_I2C_Init(); // 初始化 I2C
                   // printf("LT2991_Init\r\n");

#if LT2991_V1V2SWITCH

  I2C_WriteByte(0x06, 0x01, LT2991_ADDR);
  delay_1us(1000);
  I2C_WriteByte(0x07, 0x20, LT2991_ADDR);
  delay_1us(1000);
  I2C_WriteByte(0x08, 0x18, LT2991_ADDR);
  delay_1us(1000);
  //    I2C_WriteByte(0x09, 0x10, LT2991_ADDR);
  //		delay_1us(1000);
  I2C_WriteByte(0x01, 0xFB, LT2991_ADDR);
  delay_1us(1000);
#else

#endif
  delay_1us(1000); // 等待稳定

  uint8_t reg = I2C_ReadByte(0x01, LT2991_ADDR, 1); // 读取配置寄存器，确认配置成功
  delay_1us(1000);                                  // 等待稳定
  // printf("reg01: %02X\r\n", reg);
  reg = I2C_ReadByte(0x06, LT2991_ADDR, 1); // 读取配置寄存器，确认配置成功
  delay_1us(1000);                          // 等待稳定
  // printf("reg06: %02X\r\n", reg);
  reg = I2C_ReadByte(0x07, LT2991_ADDR, 1);
  delay_1us(1000); // 等待稳定
                   // printf("reg07: %02X\r\n", reg);
}

float LT2991_ReadCurrent_V1V2(void)
{
#if LT2991_V1V2SWITCH
  /***************单端采集电压，计算电流*******************/
  float v1, v2;
  float current;
  v1 = LT2991_ReadVoltage(0x0A);
  delay_1us(1000);
  v2 = LT2991_ReadVoltage(0x0C);
  delay_1us(1000);

  current = ((v1 - v2) * 1000 * 3.94318 - 3.0425) / 60; // 计算电流，单位 A

  // printf("V1: %.5f V  V2: %.5f V\r\n", v1, v2);
  // 	printf("V1-V2: %.5f V\r\n", v1-v2);
  // printf("Current: %.5f A\r\n", current);

  /***************差分采集电压，计算电流*******************/

  uint8_t data[1] = {0};
  uint8_t data1[1] = {0};
  uint16_t tem = 0;
  float vol_d = 0.0;

  for (int i = 0; i < 10; i++)
  {                                               // 丢弃前3次
    data[0] = I2C_ReadByte(0x0C, LT2991_ADDR, 1); // 读取 V1 寄存器
    delay_1us(1000);
    data1[0] = I2C_ReadByte(0x0D, LT2991_ADDR, 1); // 读取 V2 寄存器
    delay_1us(1000);
  }

  data[0] = I2C_ReadByte(0x0C, LT2991_ADDR, 1); // 读取 V1 寄存器
  delay_1us(1000);
  data1[0] = I2C_ReadByte(0x0D, LT2991_ADDR, 1); // 读取 V2 寄存器
  delay_1us(1000);

  // printf("0C: %02X\t 0D: %02X\r\n", data[0], data1[0]);
  tem = ((data[0] & 0xffff) << 8) | data1[0];

  tem = tem & 0x3fff;
  vol_d = (float)tem * 19.075 / 1000 * 4 / 60;
  printf("Current: %.3f A\tPower Consumption: %.3f W\r\n", vol_d, 12 * vol_d);
#else

#endif
  // 返回值测试添加
  return 1;
}

void LT2991_Read_V3V4V5V6(float *v3, float *v4, float *v5, float *v6)
{

  *v3 = LT2991_ReadVoltage(0x0E); // V3
  delay_1us(1000);                // 延时，避免连续读干扰
  *v4 = LT2991_ReadVoltage(0x10); // V4
  delay_1us(1000);                // 延时，避免连续读干扰
  *v5 = LT2991_ReadVoltage(0x12); // V5
  delay_1us(1000);                // 延时，避免连续读干扰
  *v6 = LT2991_ReadVoltage(0x14); // V6
  delay_1us(1000);                // 延时，避免连续读干扰

  printf("V3 VCORE_A: %.3f V\tV4 VCORE_B: %.3f V\tV5 VDDP: %.3f V\tV6 VDDP15: %.3f V\r\n", *v3, *v4, *v5, *v6);
}

float LT2991_ReadTemperature(uint8_t reg)
{
  float temperature = 0.0;
  uint16_t tem, tem1 = 0;
  uint8_t temdata = 0;
  uint8_t temdata1 = 0;

  temdata = I2C_ReadByte(reg, XZC2991Adr, 1);
  delay_1us(1000);
  temdata1 = I2C_ReadByte(reg + 1, XZC2991Adr, 1);
  delay_1us(1000);

  tem = ((temdata & 0x00ff) << 8) | temdata1;
  tem1 = tem & 0x1fff;
  if (tem1 >> 12)
  {
    tem1 = ((~tem1) & 0x1fff) + 1;
    temperature = 0 - (float)(tem1)*T_LSB;
  }
  else
  {
    temperature = (float)(tem1)*T_LSB;
  }

  printf("Board Temperature: %.3f ℃\r\n", temperature);
}

void LT2991_ReadAll()
{
  float v3, v4, v5, v6;
  float current = LT2991_ReadCurrent_V1V2();
  delay_1us(1000);
  LT2991_Read_V3V4V5V6(&v3, &v4, &v5, &v6);
  delay_1us(1000);
  LT2991_ReadTemperature(REG_V7_MSB);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    read2991, LT2991_ReadAll, LT2991_ReadAll);
