
#ifndef INCG_TASK_CONSOLE_H
#define INCG_TASK_CONSOLE_H
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

extern osThreadId ConsoleTaskHandle;
void ConsoleTask(void const *argument);



uint32_t bsp_gets(char pszStr[], uint32_t u32Size);
void bsp_printf(const char *format, ...);
_Bool bsp_kbhit(void);


#endif
