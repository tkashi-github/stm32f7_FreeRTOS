
#ifndef INCG_MIMIC_STRING_H
#define INCG_MIMIC_STRING_H

#if __STDC_VERSION__ < 201112L 
#error /* This Library supports C11 or later */
#endif

#include <stdint.h>
#include <stdbool.h>

#ifndef NULL
#define NULL	(0u)
#endif

static inline uint32_t mimic_strlen(char pszStr[]){
	/*-- var --*/
	uint32_t u32Cnt = 0u;

	/*-- begin --*/
	if(pszStr != NULL){
		while(pszStr[u32Cnt] != '\0'){
			u32Cnt++;
		}
	}

	return u32Cnt;
}
static inline char *mimic_strcpy(char szDst[], const char szSrc[], uint32_t u32DstSize){
	/*-- var --*/
	uint32_t u32Cnt = 0u;

	/*-- begin --*/
	if((szDst != NULL) && (szSrc != NULL)){
		while(szSrc[u32Cnt] != '\0'){
			szDst[u32Cnt] = szSrc[u32Cnt];
			u32Cnt++;
			if(u32Cnt >= u32DstSize){
				szDst[u32Cnt - 1u] = '\0';
				break;
			}
		}
	}

	return szDst;
}
#endif
