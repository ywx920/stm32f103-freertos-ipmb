
#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h"
#include "version_info.h"

// void delay_init(void);        // 初始化SysTick
// void delay_us(uint32_t us);   // 微秒级延时
// void delay_ms(uint32_t ms);   // 毫秒级延时


void System_Init(void);
void LED1_Task(void *task);		//任务1
void LED2_Task(void *task);		//任务2




#endif /* __MAIN_H */
