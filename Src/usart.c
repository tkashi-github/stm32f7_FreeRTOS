/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include "usart.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "FreeRTOS.h"
#include "RingBuffer.h"
#include "event_groups.h"
#include "timers.h"
#include "semphr.h"

static EventGroupHandle_t xStdioEventGroup;
static stu8RingBuffer_t s_stStdioRxBuf;
static stu8RingBuffer_t s_stStdioTxBuf;

static SemaphoreHandle_t s_xStdioRxSemaphore = NULL;
static SemaphoreHandle_t s_xStdioTxSemaphore = NULL;
static StaticSemaphore_t s_xStdioRxSemaphoreBuffer;
static StaticSemaphore_t s_xStdioTxSemaphoreBuffer;
static StaticEventGroup_t s_xStdioEventGroupBuffer;
/* USER CODE END 0 */

UART_HandleTypeDef huart3;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	/* USER CODE BEGIN MX_USART3_UART_Init 1 */
	/* Enable the USART Parity Error and Data Register not empty Interrupts */
	SET_BIT(huart3.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);

	/* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
	SET_BIT(huart3.Instance->CR3, USART_CR3_EIE);
	xStdioEventGroup = xEventGroupCreateStatic(&s_xStdioEventGroupBuffer);
	s_xStdioRxSemaphore = xSemaphoreCreateBinaryStatic(&s_xStdioRxSemaphoreBuffer);
	s_xStdioTxSemaphore = xSemaphoreCreateBinaryStatic(&s_xStdioTxSemaphoreBuffer);
	ClearRBu8(&s_stStdioRxBuf);
	ClearRBu8(&s_stStdioTxBuf);
	/* USER CODE END MX_USART3_UART_Init 1 */
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if (uartHandle->Instance == USART3)
	{
		/* USER CODE BEGIN USART3_MspInit 0 */

		/* USER CODE END USART3_MspInit 0 */
		/* USART3 clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();

		/**USART3 GPIO Configuration    
    PD9     ------> USART3_RX
    PD8     ------> USART3_TX 
    */
		GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		/* USART3 interrupt Init */
		HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

	if (uartHandle->Instance == USART3)
	{
		/* USER CODE BEGIN USART3_MspDeInit 0 */

		/* USER CODE END USART3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration    
    PD9     ------> USART3_RX
    PD8     ------> USART3_TX 
    */
		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_9 | GPIO_PIN_8);

		/* USART3 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		/* USER CODE BEGIN USART3_MspDeInit 1 */

		/* USER CODE END USART3_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

// console用のUARTはリングバッファを使用するように変更
void StandardIO_IntHndle(UART_HandleTypeDef *huart)
{
	uint32_t isrflags = READ_REG(huart->Instance->ISR);
	uint32_t cr1its = READ_REG(huart->Instance->CR1);
	uint32_t cr3its = READ_REG(huart->Instance->CR3);
	uint32_t errorflags;
	BaseType_t xHigherPriorityTaskWoken, xResult;

	/* If no error occurs */
	errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
	if (errorflags == RESET)
	{
		/* UART in mode Receiver ---------------------------------------------------*/
		if (((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
		{
			PushRBu8(&s_stStdioRxBuf, (uint8_t)(huart->Instance->RDR & (uint8_t)0xFFU));
			// xHigherPriorityTaskWoken must be initialised to pdFALSE.
			xHigherPriorityTaskWoken = pdFALSE;

			// Set bit 0
			xResult = xEventGroupSetBitsFromISR(
				xStdioEventGroup, // The event group being updated.
				0x00000001u, &xHigherPriorityTaskWoken);

			// Was the message posted successfully?
			if (xResult == pdPASS)
			{
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			}
			return;
		}
	}

	/* If some errors occur */
	if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
	{

		/* UART parity error interrupt occurred -------------------------------------*/
		if (((isrflags & USART_ISR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
		{
			__HAL_UART_CLEAR_IT(huart, UART_CLEAR_PEF);

			huart->ErrorCode |= HAL_UART_ERROR_PE;
		}

		/* UART frame error interrupt occurred --------------------------------------*/
		if (((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
		{
			__HAL_UART_CLEAR_IT(huart, UART_CLEAR_FEF);

			huart->ErrorCode |= HAL_UART_ERROR_FE;
		}

		/* UART noise error interrupt occurred --------------------------------------*/
		if (((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
		{
			__HAL_UART_CLEAR_IT(huart, UART_CLEAR_NEF);

			huart->ErrorCode |= HAL_UART_ERROR_NE;
		}

		/* UART Over-Run interrupt occurred -----------------------------------------*/
		if (((isrflags & USART_ISR_ORE) != RESET) &&
			(((cr1its & USART_CR1_RXNEIE) != RESET) || ((cr3its & USART_CR3_EIE) != RESET)))
		{
			__HAL_UART_CLEAR_IT(huart, UART_CLEAR_OREF);

			huart->ErrorCode |= HAL_UART_ERROR_ORE;
		}

		/* Call UART Error Call back function if need be --------------------------*/
		if (huart->ErrorCode != HAL_UART_ERROR_NONE)
		{
			/* UART in mode Receiver ---------------------------------------------------*/
			if (((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
			{
				volatile uint8_t dummy = (uint8_t)(huart->Instance->RDR & (uint8_t)huart->Mask);
			}

			/* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
			if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) ||
				(HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)))
			{
				/* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
				huart->RxState = HAL_UART_STATE_READY;

				/* Disable the UART DMA Rx request if enabled */
				if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
				{
					CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
				}
				else
				{
					/* Call user error callback */
					//HAL_UART_ErrorCallback(huart);
				}
			}
			else
			{
				/* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
				//HAL_UART_ErrorCallback(huart);
				huart->ErrorCode = HAL_UART_ERROR_NONE;
			}
		}
		return;

	} /* End if some error occurs */

	/* UART in mode Transmitter ------------------------------------------------*/
	if (((isrflags & USART_ISR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
	{
		uint8_t u8Val;
		if (GetRBu8(&s_stStdioTxBuf, &u8Val) != false)
		{
			huart->Instance->TDR = u8Val;
		}
		else
		{
			CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
		}

		return;
	}

	/* UART in mode Transmitter (transmission end) -----------------------------*/
	if (((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		//UART_EndTransmit_IT(huart);
		return;
	}
}

void RTOS_GetChar(char *ch)
{
	uint8_t u8val;

	if (ch != NULL)
	{
		while (GetRBu8(&s_stStdioRxBuf, &u8val) == false)
		{

			xEventGroupWaitBits(
					xStdioEventGroup, // The event group being tested.
					0x00000001u,	  // The bits within the event group to wait for.
					pdTRUE,			  // BIT_0  should be cleared before returning.
					pdFALSE,		  // Don't wait for both bits, either bit will do.
					portMAX_DELAY);   // Wait a maximum of 100ms for either bit to be set.
		}

		*ch = (char)u8val;
	}
}
void RTOS_PutChar(char ch)
{
	if (PushRBu8(&s_stStdioTxBuf, (uint8_t)ch) != false)
	{
		SET_BIT(huart3.Instance->CR1, USART_CR1_TXEIE);
	}
}
void RTOS_PutString(const char pszStr[])
{
	if (xSemaphoreTake(s_xStdioTxSemaphore, portMAX_DELAY) == pdTRUE)
	{
		if (pszStr != NULL)
		{
			uint32_t u32 = 0u;

			while ((pszStr[u32] != '\0') && (u32 != UINT32_MAX))
			{
				PushRBu8(&s_stStdioTxBuf, (uint8_t)pszStr[u32]);
				u32++;
			}
			if ((huart3.Instance->CR1 & USART_CR1_TXEIE) != USART_CR1_TXEIE)
			{
				SET_BIT(huart3.Instance->CR1, USART_CR1_TXEIE);
			}
		}
		xSemaphoreGive(s_xStdioTxSemaphore);
	}
}

_Bool RTOS_kbhit(void){
	return !IsRBu8EMpty(&s_stStdioRxBuf);
}

__attribute__((weak)) int __io_putchar(int ch)
{
	RTOS_PutChar((char)ch);
	return 0;
}
__attribute__((weak)) int __io_getchar(void)
{
	char ch;

	RTOS_GetChar(&ch);

	return ch;
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
