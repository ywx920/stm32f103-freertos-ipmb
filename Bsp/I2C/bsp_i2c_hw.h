#ifndef __BSP_I2C_HW_H
#define __BSP_I2C_HW_H

#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "stm32f10x_i2c.h"

/* 两路I2C引脚：
    I2C1： SCL:PB6   SDA:PB7
    I2C2： SCL:PB10  SDA:PB11 */

/**************************I2C参数定义，I2C1或I2C2********************************/
#define I2Cx I2C1
#define I2C_APBxClock_FUN RCC_APB1PeriphClockCmd
#define I2C_CLK RCC_APB1Periph_I2C1
#define I2C_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define I2C_GPIO_CLK RCC_APB2Periph_GPIOB
#define I2C_SCL_PORT GPIOB
#define I2C_SCL_PIN GPIO_Pin_6
#define I2C_SDA_PORT GPIOB
#define I2C_SDA_PIN GPIO_Pin_7

/* 新增 I2C2 */
#define I2C2_CLK RCC_APB1Periph_I2C2
#define I2C2_SCL_PIN GPIO_Pin_10
#define I2C2_SDA_PIN GPIO_Pin_11


/* STM32 I2C 快速模式 */
#define I2C_Speed 100000 //*

/* 主机地址 */
#define I2Cx_OWN_ADDRESS7 0X0A

#define I2C_PageSize 8

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/*信息输出*/
#define DEBUG_ON 0

#define INFO(fmt, arg...) printf("<<-SLAVE-INFO->> " fmt "\n", ##arg)
#define ERROR(fmt, arg...) printf("<<-SLAVE-ERROR->> " fmt "\n", ##arg)
#define DEBUG(fmt, arg...)                                              \
    do                                                                  \
    {                                                                   \
        if (DEBUG_ON)                                                   \
            printf("<<-SLAVE-DEBUG->> [%d]" fmt "\n", __LINE__, ##arg); \
    } while (0)

/* 器件地址定义 */
#define Block0_ADDRESS 0xA0 /* E2 = 0 */
// #define Block1_ADDRESS 0xA2 /* E2 = 0 */
// #define Block2_ADDRESS 0xA4 /* E2 = 0 */
// #define Block3_ADDRESS 0xA6 /* E2 = 0 */

void I2C_EE_Init(void);
void I2C_EE_BufferWrite(u8 *pBuffer, u8 WriteAddr, u16 NumByteToWrite);
uint32_t I2C_EE_ByteWrite(u8 *pBuffer, u8 WriteAddr);
uint32_t I2C_EE_PageWrite(u8 *pBuffer, u8 WriteAddr, u8 NumByteToWrite);
uint32_t I2C_EE_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_EE_WaitEepromStandbyState(void);
uint8_t I2C_Test(void);
void Hw_I2C(void);

#endif // !1
