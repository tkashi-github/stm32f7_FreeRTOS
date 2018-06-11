
#include "version.h"

#define DEF_PROGRAM "STM32F767-NUCLEO "
#define DEF_VERSION "Ver 0.1.0 "

const char g_szProgram[] = {DEF_PROGRAM};
const char g_szVersion[] = {DEF_VERSION};	/* Major.Minor.Bugfix */
const char g_szProgramVersion[] = {DEF_PROGRAM DEF_VERSION __DATE__ " " __TIME__};

