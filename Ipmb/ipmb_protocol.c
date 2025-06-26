#include "ipmb_protocol.h"
#include "bsp_gpio.h"

void IPMB_Process_Command(uint8_t* rx_buf, uint8_t* tx_buf, uint8_t* tx_len) {
    uint8_t cmd = rx_buf[0]; // 假设第3字节为命令码
    // printf("111111111\r\n");
    // printf("cmd:%02X\r\n", cmd);
    switch (cmd) {
        case 0x10: { // 获取按键状态
            // printf("Get key status\r\n");
            tx_buf[0] = GPIO_Read_Keys();
            *tx_len = 1;
            break;
        }
        case 0x11:{ // 获取电流
            // uint16_t current = LT2991_ReadCurrent_V1V2(0x0A, 0x0C); // V1, V2
            // printf("Get current\r\n");
            uint16_t current = 0xAABB; // V1, V2
            tx_buf[0] = current >> 8;
            tx_buf[1] = current & 0xFF;
            *tx_len = 2;
            break;
        }
        case 0x12: { // 获取传感器（电压/电流/温度）
            // printf("Get sensor data\r\n");
            uint16_t voltage3 = LT2991_ReadVoltage(0x0E) * 2; // V3
            uint16_t voltage4 = LT2991_ReadVoltage(0x10) * 4 / 3; // V3
            uint16_t voltage5 = LT2991_ReadVoltage(0x12); // V3
            uint16_t temperature =  LT2991_ReadTemperature(0x16); // 温度在通道1
            tx_buf[0] = voltage3 >> 8;
            tx_buf[1] = voltage3 & 0xFF;
            tx_buf[2] = voltage4 >> 8;
            tx_buf[3] = voltage4 & 0xFF;
            tx_buf[4] = voltage5 >> 8;
            tx_buf[5] = voltage5 & 0xFF;
            tx_buf[6] = temperature >> 8;
            tx_buf[7] = temperature & 0xFF;
            *tx_len = 8;
            break;
        }
        default:
        // printf("Unknown command\r\n");
            tx_buf[0] = 0x55; // 未知命令
            *tx_len = 1;
            break;
    }
}
