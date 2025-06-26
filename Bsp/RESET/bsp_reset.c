#include "bsp_reset.h"
#include "shell.h"
#include "shell_port.h"

/*  时序设计：
    1、 P12V_VPX            P3V3_AUX
    2、 P3V3_EN             P3V3_PG         10ms

    3、 P1V8_EN             P1V8_PG         20ms

    4、 VCORE_VDDQ_EN       P1V2_PHY_EN
        VCORE_VDDQ_PG       P1V2_PHY_PG     10ms

    5、 VDDP_VDDP15_EN      P2V5_PHY_EN
        VDDP_VDDP15_PG      P2V5_PHY_PG     40ms

    6、 DSP_POR_MCU                         2ms

    7、 A_PHY_RSTN          B_PHY_RSTN
        A_PCIE_RSTN         B_PCIE_RSTN     2ms

    8、 A_DSP_RSTN_MCU      B_DSP_RSTN_MCU
     */

void ResetInit(void)
{
    // 1. 开启GPIOA-C时钟
    RCC_APB2PeriphClockCmd(RCC_RST_A, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_RST_B, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_RST_C, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_RST_D, ENABLE);

    /* PB3使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 打开复用时钟
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);     // PA13,PA14
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // PA13,PA14,PA15,PB3,PB4
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);     // PB4

    /* GPIO_Mode_IN_FLOATING 输入端口配置：
    PA7，PA8.
    PB9，PB13，PB15.
    PC4，PC8，
    PD2 */

    // 2. 配置PA系列端口
    GPIO_InitTypeDef GPIOA_InitStruct;
    GPIOA_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
    GPIOA_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOA_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIOA_InitStruct);

    GPIO_InitTypeDef GPIOA2_InitStruct;
    GPIOA2_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
    GPIOA2_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIOA2_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIOA2_InitStruct);

    // 3. 配置PB系列端口
    GPIO_InitTypeDef GPIOB_InitStruct;
    GPIOB_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_12 | GPIO_Pin_14;
    GPIOB_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOB_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOB_InitStruct);

    GPIO_InitTypeDef GPIOB2_InitStruct;
    GPIOB2_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIOB2_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIOB2_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOB2_InitStruct);

    // 4. 配置PC系列端口
    GPIO_InitTypeDef GPIOC_InitStruct;
    GPIOC_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_12;
    GPIOC_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOC_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOC_InitStruct);

    GPIO_InitTypeDef GPIOC2_InitStruct;
    GPIOC2_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_8;
    GPIOC2_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIOC2_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOC2_InitStruct);

    // 5. 配置PD系列端口
    GPIO_InitTypeDef GPIOD_InitStruct;
    GPIOD_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIOD_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIOD_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIOD_InitStruct);

    GPIO_ResetBits(GPIOB, DSP_POR_MCU);
    GPIO_ResetBits(GPIOA, A_PCIE_RSTN);
    GPIO_ResetBits(GPIOB, A_DSP_RSTN_MCU);
    GPIO_ResetBits(GPIOA, A_PHY_RSTN);
    GPIO_SetBits(GPIOB, A_DSP_PCIESSEN_MCU);

    GPIO_ResetBits(GPIOA, B_PCIE_RSTN);
    GPIO_ResetBits(GPIOB, B_DSP_RSTN_MCU); // PB3
    GPIO_ResetBits(GPIOC, B_PHY_RSTN);
    GPIO_SetBits(GPIOB, B_DSP_PCIESSEN_MCU);

    GPIO_ResetBits(GPIOB, P1V8_EN);
    GPIO_ResetBits(GPIOB, VCORE_VDDQ_EN);
    GPIO_ResetBits(GPIOB, VDDP_VDDP15_EN);
    GPIO_ResetBits(GPIOC, P2V5_PHY_EN);
    GPIO_ResetBits(GPIOC, P1V2_PHY_EN);
    GPIO_ResetBits(GPIOC, P3V3_EN);
}

/* 开机启动 */
void powerReset(void)
{
    GPIO_SetBits(GPIOB, A_DSP_RSTN_MCU); // PB1
    GPIO_SetBits(GPIOB, B_DSP_RSTN_MCU); // PB3

    GPIO_SetBits(GPIOA, A_PCIE_RSTN); // TIMI0/PCIESSEN
    GPIO_SetBits(GPIOA, B_PCIE_RSTN);

    GPIO_ResetBits(GPIOB, DSP_POR_MCU); // DSP_POR_MCU先拉低 保持复位状态
    // 控制复位时序  PG输入，EN输出
    /* GPIO_SetBits拉高
    GPIO_ResetBits拉低 */
    GPIO_SetBits(GPIOC, P3V3_EN);
    while (1)
    {
        if (Bit_SET == GPIO_ReadInputDataBit(GPIOD, P3V3_PG))
        {
            delay_ms(10);
            break;
        }
    }

    GPIO_SetBits(GPIOB, P1V8_EN);
    while (1)
    {
        if (Bit_SET == GPIO_ReadInputDataBit(GPIOB, P1V8_PG))
        {
            delay_ms(20);
            break;
        }
    }

    GPIO_SetBits(GPIOB, VCORE_VDDQ_EN);
    GPIO_SetBits(GPIOC, P1V2_PHY_EN);
    while (1)
    {
        if ((Bit_SET == GPIO_ReadInputDataBit(GPIOB, VCORE_VDDQ_PG)) && (Bit_SET == GPIO_ReadInputDataBit(GPIOC, P1V2_PHY_PG)))
        {
            delay_ms(10);
            break;
        }
    }

    GPIO_SetBits(GPIOB, VDDP_VDDP15_EN);
    GPIO_SetBits(GPIOC, P2V5_PHY_EN);
    while (1)
    {
        if ((Bit_SET == GPIO_ReadInputDataBit(GPIOB, VDDP_VDDP15_PG)) && (Bit_SET == GPIO_ReadInputDataBit(GPIOC, P2V5_PHY_PG)))
        {
            delay_ms(40);
            break;
        }
    }

    GPIO_SetBits(GPIOB, DSP_POR_MCU); // 释放复位
    delay_ms(2);

    GPIO_SetBits(GPIOA, A_PHY_RSTN);
    GPIO_SetBits(GPIOC, B_PHY_RSTN);
    delay_ms(2);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    power_reset, powerReset, powerReset);

/* MCU控制其他芯片 */
void powerOn(void)
{
    /* 开机时序 */
    GPIO_SetBits(GPIOB, A_DSP_RSTN_MCU); // PB1
    GPIO_SetBits(GPIOB, B_DSP_RSTN_MCU); // PB3

    GPIO_SetBits(GPIOA, A_PCIE_RSTN); // TIMI0/PCIESSEN
    GPIO_SetBits(GPIOA, B_PCIE_RSTN);

    GPIO_ResetBits(GPIOB, DSP_POR_MCU); // DSP_POR_MCU先拉低 保持复位状态
    // 控制复位时序  PG输入，EN输出
    /* GPIO_SetBits拉高
    GPIO_ResetBits拉低 */
    GPIO_SetBits(GPIOC, P3V3_EN);
    while (1)
    {
        if (Bit_SET == GPIO_ReadInputDataBit(GPIOD, P3V3_PG))
        {
            delay_ms(10);
            break;
        }
    }

    GPIO_SetBits(GPIOB, P1V8_EN);
    while (1)
    {
        if (Bit_SET == GPIO_ReadInputDataBit(GPIOB, P1V8_PG))
        {
            delay_ms(20);
            break;
        }
    }

    GPIO_SetBits(GPIOB, VCORE_VDDQ_EN);
    GPIO_SetBits(GPIOC, P1V2_PHY_EN);
    while (1)
    {
        if ((Bit_SET == GPIO_ReadInputDataBit(GPIOB, VCORE_VDDQ_PG)) && (Bit_SET == GPIO_ReadInputDataBit(GPIOC, P1V2_PHY_PG)))
        {
            delay_ms(10);
            break;
        }
    }

    GPIO_SetBits(GPIOB, VDDP_VDDP15_EN);
    GPIO_SetBits(GPIOC, P2V5_PHY_EN);
    while (1)
    {
        if ((Bit_SET == GPIO_ReadInputDataBit(GPIOB, VDDP_VDDP15_PG)) && (Bit_SET == GPIO_ReadInputDataBit(GPIOC, P2V5_PHY_PG)))
        {
            delay_ms(40);
            break;
        }
    }

    GPIO_SetBits(GPIOB, DSP_POR_MCU); // 释放复位
    delay_ms(2);

    GPIO_SetBits(GPIOA, A_PHY_RSTN);
    GPIO_SetBits(GPIOC, B_PHY_RSTN);
    delay_ms(2);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    power_on, powerOn, powerOn);

void powerOff(void)
{
    /* 关机时序 */
    GPIO_ResetBits(GPIOB, A_DSP_RSTN_MCU); // PB1
    GPIO_ResetBits(GPIOB, B_DSP_RSTN_MCU); // PB3
    GPIO_ResetBits(GPIOA, A_PCIE_RSTN);
    GPIO_ResetBits(GPIOA, B_PCIE_RSTN);
	  GPIO_ResetBits(GPIOA, A_PHY_RSTN);
    GPIO_ResetBits(GPIOC, B_PHY_RSTN);
    GPIO_ResetBits(GPIOB, DSP_POR_MCU);

    
    // GPIO_ResetBits(GPIOC, P3V3_EN);
    GPIO_ResetBits(GPIOB, P1V8_EN);
    GPIO_ResetBits(GPIOB, VCORE_VDDQ_EN);
    GPIO_ResetBits(GPIOB, VDDP_VDDP15_EN);

    GPIO_ResetBits(GPIOC, P1V2_PHY_EN);
    GPIO_ResetBits(GPIOC, P2V5_PHY_EN);

    // delay_ms(10);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    power_off, powerOff, powerOff);

void soft_reset(void)
{
    /* 开机时序 */
    GPIO_ResetBits(GPIOB, A_DSP_RSTN_MCU); // PB1
    GPIO_ResetBits(GPIOB, B_DSP_RSTN_MCU); // PB3
    GPIO_ResetBits(GPIOA, A_PCIE_RSTN);
    GPIO_ResetBits(GPIOA, B_PCIE_RSTN);
    GPIO_ResetBits(GPIOB, DSP_POR_MCU);
    GPIO_ResetBits(GPIOA, A_PHY_RSTN);
    GPIO_ResetBits(GPIOC, B_PHY_RSTN);

    delay_ms(500);

    GPIO_SetBits(GPIOB, A_DSP_RSTN_MCU);
    GPIO_SetBits(GPIOB, B_DSP_RSTN_MCU);
    GPIO_SetBits(GPIOA, A_PCIE_RSTN);
    GPIO_SetBits(GPIOA, B_PCIE_RSTN);
    GPIO_SetBits(GPIOB, DSP_POR_MCU);
    delay_ms(50);

    GPIO_SetBits(GPIOA, A_PHY_RSTN);
    GPIO_SetBits(GPIOC, B_PHY_RSTN);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    soft_reset, soft_reset, soft_reset);
