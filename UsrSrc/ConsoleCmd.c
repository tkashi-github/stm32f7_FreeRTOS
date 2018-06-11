
#include "ConsoleCmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"

uint32_t bsp_gets(char pszStr[], uint32_t u32Size);
void bsp_printf(const char *format, ...);


static void CmdHelp(uint32_t argc, const char *argv[]);
static void CmdVersion(uint32_t argc, const char *argv[]);

stCmdTable_t g_stCmdTable[] = {
	{"HELP",        CmdHelp,        "Help"},	/* Help Command*/
	{"VERSION",     CmdVersion,     "Version"},	/* Version Command*/
	
	{NULL, NULL, NULL},	/* Terminator */
};

static void CmdHelp(uint32_t argc, const char *argv[]){
	/*-- var --*/
	uint32_t u32 = 0u;

	/*-- begin --*/
	bsp_printf("\r\n[%s (%d)] HELP\r\n", __FUNCTION__, __LINE__);

	while(g_stCmdTable[u32].m_pszCmd != NULL){
		bsp_printf("%03lu : %-20s, %-20s\r\n", u32,g_stCmdTable[u32].m_pszCmd,g_stCmdTable[u32].m_pszComments);
		u32++;
	}
	bsp_printf("[%s (%d)] ----\r\n", __FUNCTION__, __LINE__);
	return;
}


static void CmdVersion(uint32_t argc, const char *argv[]){
	bsp_printf("\r\n\r\nVersion = %s\r\n", g_szProgramVersion);

	bsp_printf("Compiler Version\r\n");

#ifdef __GNUC__
	bsp_printf("GCC %d.%d.%d\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
	bsp_printf("Unkown Compiler\r\n\r\n");
#endif
}
