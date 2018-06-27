


#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TaskConsole.h"
#include "version.h"
#include "ConsoleCmd.h"


osThreadId ConsoleTaskHandle;
osStaticThreadDef_t ConsoleTaskTCB;
uint32_t ConsoleTaskStack[8192/sizeof(uint32_t)];

#define DEF_PROMPT "STM32F7 NUCLEO > "
extern __attribute__((weak)) int __io_getchar(void);
extern __attribute__((weak)) int __io_putchar(int ch);


uint32_t bsp_gets(char pszStr[], uint32_t u32Size){
	uint32_t u32Cnt = 0u;

	if((pszStr != NULL) && (u32Size > 0u)){
		_Bool bReturnCode = false;

		memset(pszStr, 0, u32Size);
		
		while(bReturnCode == false){
			char ch;
			ch = __io_getchar();
			
			
			switch(ch){
			case '\b':	// バックスペース
				if(u32Cnt > 0u){
					u32Cnt--;
					pszStr[u32Cnt] = '\0';
					__io_putchar('\b');
					__io_putchar(' ');
					__io_putchar('\b');
				}
				break;
			case '\r':		// TeraTermの改行コードは "CR"設定ではCRのみ送られてくる（CRLFにならない）
				//u32Cnt--;			
				pszStr[u32Cnt] = '\0';
				bReturnCode = true;
				__io_putchar((char)ch);
				__io_putchar('\n');
				break;
			default:
				pszStr[u32Cnt] = ch;
				u32Cnt++;
				if(u32Cnt >= u32Size){
					u32Cnt--;
					pszStr[u32Cnt] = '\0';
					break;
				}
				__io_putchar((char)ch);
				break;
			}
		}
	}

	return u32Cnt;
}

extern void RTOS_PutString(const char pszStr[]);

void bsp_printf(const char *format, ...){
	va_list arg;
	char szStr[512];

    va_start(arg, format);
    vsnprintf(szStr, sizeof(szStr), format, arg);
    va_end(arg);

	RTOS_PutString(szStr);

}
extern _Bool RTOS_kbhit(void);

_Bool bsp_kbhit(void){
	return RTOS_kbhit();
}


void ConsoleTask(void const *argument){
	char szBuf[512];

	bsp_printf("\r\n\r\nVersion = %s\r\n", g_szProgramVersion);

	bsp_printf("\r\n\r\nCompiler Version\r\n");

#ifdef __GNUC__
	bsp_printf("GCC %d.%d.%d\r\n\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
	bsp_printf("Unkown Compiler\r\n\r\n");
#endif
	bsp_printf("%s", DEF_PROMPT);

	//osThreadSuspend(osThreadGetId());
	for(;;){
		uint32_t u32;
		uint32_t argc;
		char *argv[16];
		
		bsp_gets(szBuf, 512);
		
		argc = 0u;
		argv[argc] = strtok(szBuf, " "); // 1個目の部分文字列取得
    	while (argv[argc] != NULL) {
			argc++;
			argv[argc] = strtok(NULL, " "); // 1個目の部分文字列取得
			if(argc >= 16u){
				break;
			}
		}

		u32 = 0u;
		while(g_stCmdTable[u32].m_pszCmd != NULL){
			if(stricmp(g_stCmdTable[u32].m_pszCmd, argv[0]) == 0){
				g_stCmdTable[u32].m_pfnc(argc, (const char **)argv);
				break;
			}
			u32++;
		}
		bsp_printf("%s", DEF_PROMPT);
	}
}
