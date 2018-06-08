


#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TaskConsole.h"


osThreadId ConsoleTaskHandle;
#define DEF_PROMPT "STM32F7 NUCLEO > "
extern __attribute__((weak)) int __io_getchar(void);


static uint32_t bsp_gets(char pszStr[], uint32_t u32Size);
static void bsp_printf(const char *format, ...);

static uint32_t bsp_gets(char pszStr[], uint32_t u32Size){
	uint32_t u32Cnt = 0u;

	if((pszStr != NULL) && (u32Size > 0u)){
		_Bool bReturnCode = false;

		memset(pszStr, 0, u32Size);
		
		while(bReturnCode == false){
			char ch;
			ch = __io_getchar();
			
			pszStr[u32Cnt] = ch;
			u32Cnt++;
			if(u32Cnt >= u32Size){
				u32Cnt--;
				pszStr[u32Cnt] = '\0';
				break;
			}
			switch(ch){
			case '\b':
				if(u32Cnt > 0u){
					u32Cnt--;
					pszStr[u32Cnt] = '\0';
				}
				break;
			case '\r':
				bReturnCode = true;
				break;
			default:
				break;
			}
		}
	}

	return u32Cnt;
}

extern void RTOS_PutString(const char pszStr[]);

static void bsp_printf(const char *format, ...){
	va_list arg;
	char szStr[512];

    va_start(arg, format);
    vsnprintf(szStr, sizeof(szStr), format, arg);
    va_end(arg);

	RTOS_PutString(szStr);

}

void ConsoleTask(void const *argument){
	char szBuf[512];

	bsp_printf("\r\n\r\nCompiler Version\r\n");

#ifdef __GNUC__
	bsp_printf("GCC %d.%d.%d\r\n\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
	bsp_printf("Unkown Compiler\r\n\r\n");
#endif
	bsp_printf("%s", DEF_PROMPT);

	//osThreadSuspend(osThreadGetId());
	for(;;){
		bsp_gets(szBuf, 512);

		bsp_printf("%s", DEF_PROMPT);
	}
}
