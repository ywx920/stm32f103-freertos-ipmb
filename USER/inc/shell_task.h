/**
 * @file shell_task.h
 * @brief Shell 处理任务头文件
 */

#ifndef __SHELL_TASK_H
#define __SHELL_TASK_H

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief 创建 Shell 处理任务
 * @param 无
 * @retval pdTRUE: 成功, pdFALSE: 失败
 */
BaseType_t xShellTaskCreate(void);

#endif /* __SHELL_TASK_H */
