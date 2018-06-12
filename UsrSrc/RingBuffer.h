#ifndef INCG_RING_BUFFER_H
#define INCG_RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define DEF_RB_LEN	(1024U)
typedef struct{
	uint32_t rp;
	uint32_t wp;
	uint8_t u8Data[DEF_RB_LEN];
}stu8RingBuffer_t;


_Bool PushRBu8(stu8RingBuffer_t *pRB, uint8_t u8Val);
_Bool GetRBu8(stu8RingBuffer_t *pRB, uint8_t *pu8Val);
_Bool ClearRBu8(stu8RingBuffer_t *pRB);
_Bool IsRBu8EMpty(stu8RingBuffer_t *pRB);
#endif
