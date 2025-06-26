#include "bsp_gpio.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "shell.h"
#include "shell_port.h"

void GPIO_Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // GPIO_InitTypeDef gpio;
    // gpio.GPIO_Mode = GPIO_Mode_IPU;
    // gpio.GPIO_Speed = GPIO_Speed_50MHz;
    // for (int i = 0; i <= 6; ++i) {
    //     gpio.GPIO_Pin = (1 << i);
    //     GPIO_Init(GPIOA, &gpio);
    // }

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void printBinary(uint8_t value)
{
    for (int i = 7; i >= 0; --i)
    {
        printf("GA value :%d\r\n", (value >> i) & 1);
    }
}

uint8_t GPIO_Read_Keys(void)
{
    // uint8_t keys = 0;
    // for (int i = 0; i <= 6; ++i) {
    //     if (!GPIO_ReadInputDataBit(GPIOA, 1 << i)) {
    //         keys |= (1 << i);
    //     }
    // }
    // printf("11keys: %02X\r\n", keys); // 调试输出
    // // 这里可以添加其他处理逻辑，比如发送到上位机等
    // return keys;
    // GPIOA1~A5: GA0~GA4
    // GPIOA6: GAP
    uint8_t ga_value = 0;
    uint8_t gap = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6); // GAP

    if (gap == 0)
    {
        printf("GAP value = 0\r\n");
        // return 0;
    }
    else
    {
        printf("GAP value = 1\r\n");
    }

    // 读取 GA[4:0]
    ga_value |= (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) << 4); // GA4
    ga_value |= (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) << 3); // GA3
    ga_value |= (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) << 2); // GA2
    ga_value |= (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) << 1); // GA1
    ga_value |= (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) << 0); // GA0

    // uint8_t hardware_address = 0x40 | ga_value;
    // uint8_t ipmb_address = hardware_address << 1;

    printf("GA value: 0x%02X\r\n", ga_value);
    // printBinary(ga_value);
    // printf("Hardware Address: 0x%02X\r\n", hardware_address);
    // printf("IPMB Address: 0x%02X\r\n", ipmb_address);

    // return ipmb_address;
    return 1;
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
read_keys, GPIO_Read_Keys, read keys);


