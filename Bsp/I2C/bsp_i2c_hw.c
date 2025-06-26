#include "bsp_i2c_hw.h"
#include <stdio.h>

/**
 ******************************************************************************
 * @file    bsp_i2c_ee.c
 * @author  STMicroelectronics
 * @version V1.0
 * @date    2013-xx-xx
 * @brief   i2c EEPROM(AT24C02)应用函数bsp
 ******************************************************************************
 * @attention
 ******************************************************************************
 */

uint16_t ADDRESS;

static __IO uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

#define EEP_Firstpage 0x00
uint8_t I2c_Buf_Write[256];
uint8_t I2c_Buf_Read[256];

/**
 * @brief  I2C I/O配置
 * @param  无
 * @retval 无
 */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能与 I2C 有关的时钟 */
  I2C_APBxClock_FUN(I2C_CLK, ENABLE);
  I2C_GPIO_APBxClock_FUN(I2C_GPIO_CLK, ENABLE);

  /* I2C_SCL、I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
  GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
  GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
}

/**
 * @brief  I2C 工作模式配置
 * @param  无
 * @retval 无
 */
static void I2C_Mode_Configu(void)
{
  I2C_InitTypeDef I2C_InitStructure;

  /* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;

  /* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;

  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;

  /* I2C的寻址模式 */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  /* 通信速率 */
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

  /* I2C 初始化 */
  I2C_Init(I2Cx, &I2C_InitStructure);

  /* 使能 I2C */
  I2C_Cmd(I2Cx, ENABLE);
}

/**
 * @brief  I2C 外设(EEPROM)初始化
 * @param  无
 * @retval 无
 */
void I2C_EE_Init(void)
{

  I2C_GPIO_Config();

  I2C_Mode_Configu();

/* 选择从设备的地址 */
#ifdef Block0_ADDRESS
  ADDRESS = Block0_ADDRESS;
#endif

#ifdef Block1_ADDRESS
  ADDRESS = Block1_ADDRESS;
#endif

#ifdef Block2_ADDRESS
  ADDRESS = Block2_ADDRESS;
#endif

#ifdef Block3_ADDRESS
  ADDRESS = Block3_ADDRESS;
#endif
}

/**
 * @brief   将缓冲区中的数据写到I2C从设备中
 * @param
 *		@arg pBuffer:缓冲区指针
 *		@arg WriteAddr:写地址
 *     @arg NumByteToWrite:写的字节数
 * @retval  无
 */
void I2C_EE_BufferWrite(u8 *pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage = NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;

  /* If WriteAddr is I2C_PageSize aligned  */
  if (Addr == 0)
  {
    /* If NumByteToWrite < I2C_PageSize */
    if (NumOfPage == 0)
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      while (NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if (NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else
  {
    /* If NumByteToWrite < I2C_PageSize */
    if (NumOfPage == 0)
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage = NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;

      if (count != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      }

      while (NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += I2C_PageSize;
        pBuffer += I2C_PageSize;
      }
      if (NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
}

/**
 * @brief   写一个字节到I2C设备中
 * @param
 *		@arg pBuffer:缓冲区指针
 *		@arg WriteAddr:写地址
 * @retval  无
 */
uint32_t I2C_EE_ByteWrite(u8 *pBuffer, u8 WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(0);
  }

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2Cx, ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(1);
  }
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2Cx, WriteAddr);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(2);
  }

  /* Send the byte to be written */
  I2C_SendData(I2Cx, *pBuffer);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(3);
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  return 1;
}

/**
 * @brief   写多个字节到I2C设备中
 * @param
 *		@arg pBuffer:缓冲区指针
 *		@arg WriteAddr:写地址
 *     @arg NumByteToWrite:写的字节数
 * @retval  无
 */
uint32_t I2C_EE_PageWrite(u8 *pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  I2CTimeout = I2CT_LONG_TIMEOUT;

  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(4);
  }

  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(5);
  }

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2Cx, ADDRESS, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(6);
  }

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2Cx, WriteAddr);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(7);
  }

  /* While there is data to be written */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    I2C_SendData(I2Cx, *pBuffer);

    /* Point to the next byte to be written */
    pBuffer++;

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
      if ((I2CTimeout--) == 0)
        return I2C_TIMEOUT_UserCallback(8);
    }
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  return 1;
}

/**
 * @brief   从I2C从设备里面读取一块数据
 * @param
 *		@arg pBuffer:存放从读取出的数据的缓冲区指针
 *		@arg WriteAddr:数据位于从设备的地址
 *     @arg NumByteToWrite:要从设备读取的字节数
 * @retval  无
 */
uint32_t I2C_EE_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead)
{

  I2CTimeout = I2CT_LONG_TIMEOUT;

  //*((u8 *)0x4001080c) |=0x80;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(9);
  }

  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(10);
  }

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2Cx, ADDRESS, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(11);
  }

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2Cx, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2Cx, ReadAddr);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(12);
  }

  /* Send STRAT condition a second time */
  I2C_GenerateSTART(I2Cx, ENABLE);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(13);
  }

  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2Cx, ADDRESS, I2C_Direction_Receiver);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if ((I2CTimeout--) == 0)
      return I2C_TIMEOUT_UserCallback(14);
  }

  /* While there is data to be read */
  while (NumByteToRead)
  {
    if (NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2Cx, DISABLE);

      /* Send STOP Condition */
      I2C_GenerateSTOP(I2Cx, ENABLE);
    }

    /* Test on EV7 and clear it */
    I2CTimeout = I2CT_LONG_TIMEOUT;

    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
    {
      if ((I2CTimeout--) == 0)
        return I2C_TIMEOUT_UserCallback(3);
    }
    {
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2Cx);

      /* Point to the next location where the byte read will be saved */
      pBuffer++;

      /* Decrement the read bytes counter */
      NumByteToRead--;
    }
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2Cx, ENABLE);

  return 1;
}

/**
 * @brief  Wait for SLAVE Standby state
 * 检测从设备的空闲状态，以便判断其是否准备好接收新的数据写入操作
 * 在写入缓冲区数据时使用
 * @param  无
 * @retval 无
 */
void I2C_EE_WaitEepromStandbyState(void)
{
  vu16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART(I2Cx, ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2Cx, ADDRESS, I2C_Direction_Transmitter);
  } while (!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));

  /* Clear AF flag */
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
  /* STOP condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);
}

/**
 * @brief  Basic management of the timeout situation.
 * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
 * @retval 返回0，表示IIC读取失败.
 */
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  ERROR("I2C 等待超时!errorCode = %d", errorCode);

  return 0;
}

/**
 * @brief  I2C(AT24C02)读写测试
 * @param  无
 * @retval 正常返回1，异常返回0
 */
uint8_t I2C_Test(void)
{
  uint16_t i;

  printf("写入的数据\n\r");

  for (i = 0; i <= 255; i++) // 填充缓冲
  {
    I2c_Buf_Write[i] = i;

    printf("0x%02X ", I2c_Buf_Write[i]);
    if (i % 16 == 15)
      printf("\n\r");
  }

  // 将I2c_Buf_Write中顺序递增的数据写入EERPOM中
  I2C_EE_BufferWrite(I2c_Buf_Write, EEP_Firstpage, 256);

  INFO("\n\r写成功\n\r");

  INFO("\n\r读出的数据\n\r");
  // 将EEPROM读出数据顺序保持到I2c_Buf_Read中
  I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);

  // 将I2c_Buf_Read中的数据通过串口打印
  for (i = 0; i < 256; i++)
  {
    if (I2c_Buf_Read[i] != I2c_Buf_Write[i])
    {
      ERROR("0x%02X ", I2c_Buf_Read[i]);
      ERROR("错误:I2C EEPROM写入与读出的数据不一致\n\r");
      return 0;
    }
    printf("0x%02X ", I2c_Buf_Read[i]);
    if (i % 16 == 15)
      printf("\n\r");
  }
  INFO("I2C(AT24C02)读写测试成功\n\r");

  return 1;
}

void Hw_I2C(void)
{
  // EEPROM 读写测试
  if (I2C_Test() == 1)
  {
    printf("\r\n Hw_I2C Test OK \r\n");
  }
  else
  {
    printf("\r\n Hw_I2C Test Error \r\n");
  }
}









/*********************************************END OF FILE**********************/
