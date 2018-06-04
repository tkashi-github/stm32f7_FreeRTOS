
#ifndef INCG_CPU__FUNC_H
#define INCG_CPU__FUNC_H

#include <stdint.h>
/* ignore some GCC warnings */
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

__attribute__( ( always_inline ) ) static __inline uint32_t DisableIRQ(void)
{
	uint32_t result;

	__asm volatile ("MRS %0, primask" : "=r" (result) );
	__asm volatile ("cpsid i" : : : "memory");
    return(result);
}

__attribute__( ( always_inline ) ) static __inline void SetIRQ(uint32_t priMask)
{
	__asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

#endif
