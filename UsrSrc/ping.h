
#ifndef INCG_PING_H
#define INCG_PING_H
#include <stdint.h>
#include <stdbool.h>

_Bool ping(uint32_t destIP);
void CmdPing(uint32_t argc, const char *argv[]);

#endif
