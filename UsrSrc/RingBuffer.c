

#include <stdio.h>
#include "./RingBuffer.h"
#include "./CPUFunc.h"

_Bool PushRBu8(stu8RingBuffer_t *pRB, uint8_t u8Val)
{
	/*-- var --*/
	_Bool bret = false;

	/*-- begin --*/
	if (pRB != NULL)
	{
		uint32_t cpsr = DisableIRQ();

		pRB->u8Data[pRB->wp] = u8Val;
		pRB->wp++;
		pRB->wp %= DEF_RB_LEN;
		if (pRB->wp == pRB->rp)
		{
			pRB->rp++;
			pRB->rp %= DEF_RB_LEN;
		}

		SetIRQ(cpsr);
		bret = true;
	}
	return bret;
}

_Bool GetRBu8(stu8RingBuffer_t *pRB, uint8_t *pu8Val)
{
	/*-- var --*/
	_Bool bret = false;

	/*-- begin --*/
	if ((pRB != NULL) && (pu8Val != NULL))
	{
		uint32_t cpsr = DisableIRQ();
		uint32_t nextrp = pRB->rp + 1;
		nextrp %= DEF_RB_LEN;
		if (pRB->wp != nextrp)
		{
			*pu8Val = pRB->u8Data[pRB->rp];
			pRB->rp++;
			pRB->rp %= DEF_RB_LEN;
			if (pRB->wp == pRB->rp)
			{
				pRB->wp++;
				pRB->wp %= DEF_RB_LEN;
			}
			bret = true;
		}
		SetIRQ(cpsr);
		
	}
	return bret;
}

_Bool ClearRBu8(stu8RingBuffer_t *pRB)
{
	/*-- var --*/
	_Bool bret = false;

	/*-- begin --*/
	if (pRB != NULL)
	{
		uint32_t cpsr = DisableIRQ();

		pRB->wp = 0u;
		pRB->rp = 0u;

		SetIRQ(cpsr);
		bret = true;
	}
	return bret;
}