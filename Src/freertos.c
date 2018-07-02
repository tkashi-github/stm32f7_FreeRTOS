/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "TaskConsole.h"
#include "CPUFunc.h"
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOSConfig.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
typedef enum
{
	enLED0,
	enLED1,
	enLED2,
	enLED_MAX,
} enLedNo_t;

GPIO_TypeDef *LEDPort = GPIOB;

const uint16_t u16LEDPin[enLED_MAX] = {
	GPIO_PIN_0,
	GPIO_PIN_7,
	GPIO_PIN_14,
};
const uint32_t u32LEDBlinkTime[enLED_MAX] = {
	250u,
	500u,
	1000u};
osThreadId LEDTaskHandle[enLED_MAX];
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const *argument);

extern void MX_USB_DEVICE_Init(void);
extern void MX_FATFS_Init(void);
extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void LEDTask(void const *argument);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void GetRunCount(uint32_t *p32Last, uint32_t *pu32Max);

static uint32_t s_u32CurrentRun = 0u;
static uint32_t s_u32LastRun = 0u;
static uint32_t s_u32RunMax = 0u;

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
	#if 1
	uint32_t primask = DisableIRQ();

	s_u32CurrentRun++;
	SetIRQ(primask);
	#endif
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
_Bool g_bInitEnd = false;
__weak void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
#if 1
	static uint32_t cnt = 0u;
	if(g_bInitEnd != false){
		if (cnt >= 1000u)
		{
			cnt = 0u;
			uint32_t primask = DisableIRQ();

			s_u32LastRun = s_u32CurrentRun;
			s_u32CurrentRun = 0u;
			if (s_u32LastRun > s_u32RunMax)
			{
				s_u32RunMax = s_u32LastRun;
			}
			SetIRQ(primask);
		}else{
			cnt++;
		}
	}else{
		s_u32CurrentRun = 0u;
	}
#endif
}

void GetRunCount(uint32_t *p32Last, uint32_t *pu32Max)
{
	if ((p32Last != NULL) && (pu32Max != NULL))
	{
		uint32_t primask = DisableIRQ();
		*p32Last = s_u32LastRun;
		*pu32Max = s_u32RunMax;
		SetIRQ(primask);
	}
}

/* USER CODE END 3 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */

	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN 4 */
typedef struct{
	osThreadId    *pOsId;
	osThreadDef_t OsDef;
	void          *argment;
}stOSdefTable_t;

StaticTask_t LEDTaskTCB[enLED_MAX];
uint32_t LEDTaskStack[enLED_MAX][1024/sizeof(uint32_t)];

static const stOSdefTable_t g_stOSTable[] = {
	{
		&ConsoleTaskHandle,
		{"ConsoleTask", ConsoleTask, osPriorityBelowNormal, 1, 8192, ConsoleTaskStack, &ConsoleTaskTCB},
		0,
	},
	{
		&LEDTaskHandle[enLED0],
		{"LEDTask0", LEDTask, osPriorityLow, 1, 1024, LEDTaskStack[enLED0], &LEDTaskTCB[enLED0]},
		(void *)enLED0,
	},
	{
		&LEDTaskHandle[enLED1],
		{"LEDTask1", LEDTask, osPriorityLow, 1, 1024, LEDTaskStack[enLED1], &LEDTaskTCB[enLED1]},
		(void *)enLED1,
	},
	{
		&LEDTaskHandle[enLED2],
		{"LEDTask2", LEDTask, osPriorityLow, 1, 1024, LEDTaskStack[enLED2], &LEDTaskTCB[enLED2]},
		(void *)enLED2,
	},
	// Terminate
	{	
		NULL,
		{0},
		0,
	},
};
#if (configSUPPORT_STATIC_ALLOCATION == 1)
/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif

/* USER CODE END 4 */
/* StartDefaultTask function */
void StartDefaultTask(void const *argument)
{

	/* init code for USB_DEVICE */
	//MX_USB_DEVICE_Init();

	/* init code for FATFS */
	//MX_FATFS_Init();

	/* init code for LWIP */
	//MX_LWIP_Init();

	/* USER CODE BEGIN StartDefaultTask */
	{
		uint32_t i=0;

		while(g_stOSTable[i].pOsId != NULL){
			*g_stOSTable[i].pOsId = osThreadCreate(&g_stOSTable[i].OsDef, g_stOSTable[i].argment);
			i++;
		}
		vTaskDelay(1000u);
		g_bInitEnd = true;
		osThreadSuspend(osThreadGetId());
	}
	/* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
void LEDTask(void const *argument)
{
	if ((uint32_t)argument >= (uint32_t)enLED_MAX)
	{
		osThreadSuspend(osThreadGetId());
	}

	/* Infinite loop */
	for (;;)
	{
		//printf("[%s (%d)] LED%luTask (%lu msec)\r\n", __FUNCTION__, __LINE__, (uint32_t)argument, xTaskGetTickCount());
		HAL_GPIO_TogglePin(LEDPort, u16LEDPin[(uint32_t)argument]);
		vTaskDelay(u32LEDBlinkTime[(uint32_t)argument]);
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
