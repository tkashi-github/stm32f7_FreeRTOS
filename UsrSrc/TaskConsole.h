
#ifndef INCG_TASK_CONSOLE_H
#define INCG_TASK_CONSOLE_H
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

extern osThreadId ConsoleTaskHandle;
extern osStaticThreadDef_t ConsoleTaskTCB;
extern uint32_t ConsoleTaskStack[];

void ConsoleTask(void const *argument);



extern uint32_t bsp_gets(char pszStr[], uint32_t u32Size);
extern void bsp_printf(const char *format, ...);
extern _Bool bsp_kbhit(void);


#endif
