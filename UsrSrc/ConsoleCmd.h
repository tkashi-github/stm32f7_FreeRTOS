

#ifndef INCG_CONSOLE_CMD_H
#define INCG_CONSOLE_CMD_H

#include <stdint.h>
#include <stdbool.h>
typedef void (*pfnCmd)(uint32_t argc, const char *argv[]);

typedef struct{
	const char      *m_pszCmd;
	const pfnCmd    m_pfnc;
	const char      *m_pszComments;	
}stCmdTable_t;


extern stCmdTable_t g_stCmdTable[];

#endif
