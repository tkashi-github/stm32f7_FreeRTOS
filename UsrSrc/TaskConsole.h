
#ifndef INCG_TASK_CONSOLE_H
#define INCG_TASK_CONSOLE_H
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

extern osThreadId ConsoleTaskHandle;
void ConsoleTask(void const *argument);

#endif
