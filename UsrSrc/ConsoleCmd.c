
#include "ConsoleCmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "FreeRTOS.h"
#include "task.h"
#include "CPUFunc.h"
#include "ping.h"
#include <stdatomic.h>

extern uint32_t bsp_gets(char pszStr[], uint32_t u32Size);
extern void bsp_printf(const char *format, ...);
extern _Bool bsp_kbhit(void);
extern void GetRunCount(uint32_t *p32Last, uint32_t *pu32Max);

static void CmdHelp(uint32_t argc, const char *argv[]);
static void CmdVersion(uint32_t argc, const char *argv[]);
static void CmdArg(uint32_t argc, const char *argv[]);
static void CmdTick(uint32_t argc, const char *argv[]);
static void CmdLoad(uint32_t argc, const char *argv[]);
static void CmdAtomicTest(uint32_t argc, const char *argv[]);

stCmdTable_t g_stCmdTable[] = {
	{"HELP",        CmdHelp,        "Help"},	/* Help Command*/
	{"VERSION",     CmdVersion,     "Version"},	/* Version Command*/
	{"ARG",         CmdArg,         "Argment Test"},	/* Version Command*/
	{"TICK",        CmdTick,        "Tick Test"},	/* Version Command*/
	{"LOAD",        CmdLoad,        "CPU Load"},	/* Version Command*/
	{"PING",        CmdPing,        "Ping"},	/* Version Command*/
	{"ATOMIC",      CmdAtomicTest,  "Atomic Test"},	/* Version Command*/
	{NULL, NULL, NULL},	/* Terminator */
};

static void CmdHelp(uint32_t argc, const char *argv[]){
	/*-- var --*/
	uint32_t u32 = 0u;

	/*-- begin --*/
	bsp_printf("\r\n[%s (%d)] HELP\r\n", __FUNCTION__, __LINE__);

	while(g_stCmdTable[u32].m_pszCmd != NULL){
		bsp_printf("%03lu : %-20s %-20s\r\n", u32,g_stCmdTable[u32].m_pszCmd,g_stCmdTable[u32].m_pszComments);
		u32++;
	}
	bsp_printf("[%s (%d)] ----\r\n", __FUNCTION__, __LINE__);
	return;
}


static void CmdVersion(uint32_t argc, const char *argv[]){
	bsp_printf("\r\n\r\n%s\r\n", g_szProgramVersion);

	bsp_printf("\r\nCompiler Version\r\n");

#ifdef __GNUC__
	bsp_printf("GCC %d.%d.%d\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
	bsp_printf("Unkown Compiler\r\n\r\n");
#endif
}


static void CmdArg(uint32_t argc, const char *argv[]){
	bsp_printf("Argment Test\r\n");

	for(uint32_t i=0u;i<argc;i++){
		bsp_printf("argc = %02lu : %s\r\n", i,argv[i]);
	}
}

static void CmdTick(uint32_t argc, const char *argv[]){
	TickType_t tick;
	bsp_printf("Tick Test\r\n");
	
	tick = xTaskGetTickCount();
	while(bsp_kbhit() == false){
		bsp_printf("tick = %lu msec\r\n", tick);
		vTaskDelayUntil((TickType_t *const)&tick, 1000);
	}
}
static void CmdLoad(uint32_t argc, const char *argv[]){
	TickType_t tick;
	uint32_t LastRun, MaxRun;
	bsp_printf("CPU Load\r\n");
	
	tick = xTaskGetTickCount();
	while(bsp_kbhit() == false){
		GetRunCount(&LastRun, &MaxRun);
		bsp_printf("Load = %f [%lu, %lu](%lu msec)\r\n", 1.0 - (double)LastRun/(double)MaxRun, LastRun, MaxRun, tick);
		vTaskDelayUntil((TickType_t *const)&tick, 1000);
	}
}

_Atomic __attribute__ ((aligned(64))) uint32_t g_u32Atomic;
uint32_t g_u32NoAtomic;

static void CmdAtomicTest(uint32_t argc, const char *argv[]){
	uint32_t tmp1, tmp2;
	TickType_t tick;
	atomic_init(&g_u32Atomic, 0);
	g_u32NoAtomic = 0;

	tick = xTaskGetTickCount();
	while(bsp_kbhit() == false){
		uint32_t cpsr = DisableIRQ();
		tmp1 = g_u32Atomic;
		tmp2 = g_u32NoAtomic;
		SetIRQ(cpsr);
		bsp_printf("g_u32Atomic   = %lu : g_u32NoAtomic = %lu (%lu msec)\r\n", tmp1, tmp2, tick);
		vTaskDelayUntil((TickType_t *const)&tick, 1000);
	}
}

