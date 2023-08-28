/**
 * @file bsp_common.c
 * @author ygl
 * @brief 
 * @version 0.1
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PRINTF_BUFFERS_LEN 		1024		//usart_printf 缓冲区大小
#define PRINTF_UART				huart1		//printf 默认串口

// 72M 频率，for循环 5 次约 1 us
#define DELAY_US_COUNT	5
extern UART_HandleTypeDef PRINTF_UART;
static uint8_t ch;

//uint16_t 类型置位、清除
#define SET_EVENT(event, arg)               ATOMIC_SETH_BIT((event), (arg))
#define CLEAR_EVENT(event, arg)             ATOMIC_CLEARH_BIT((event), (arg))
#define GET_EVEVT(event, arg)               ((event) & (arg))

#ifdef __GNUC__
//重写这个函数,重定向printf函数到串口
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&PRINTF_UART, (uint8_t*)&ch, 1, 0xFFFF);
	return ch;
}
#elif
/*fputc*/
int fputc(int c, FILE * f)
{
	ch = c;
	HAL_UART_Transmit(&PRINTF_UART, (uint8_t*)&ch, 1, 1000);
	return c;
}
#endif


void usart_printf(UART_HandleTypeDef *USARTx, const char *fmt, ...)
{
	va_list ap;
	char bufs[PRINTF_BUFFERS_LEN];

	va_start(ap, fmt);
	vsnprintf(bufs, sizeof(bufs), fmt, ap);
	va_end(ap);

	HAL_UART_Transmit(USARTx, bufs, strlen(bufs), 0xffff);
}


void delay_us(uint32_t count)
{
	for(uint32_t i = 0; i < count; ++i)
	{
		for(uint8_t j = 0; j < 5; ++j);
	}
}


void delay_ms(uint32_t count)
{
	for(uint32_t i = 0; i < count; ++i)
		delay_us(1000);
}


#ifdef OS_USED
void hal_thread_create(volatile void* threadId, uint16_t stackSize, int Priority, void (*fn)(void*), void* arg)
{
	const osThreadAttr_t Task_attributes = {
	  .name = "parseTask",
	  .stack_size = stackSize,
	  .priority = (osPriority_t) Priority,
	};
  	threadId = osThreadNew(fn, NULL, &Task_attributes);

	(void)threadId; //Eliminate warning
}

void hal_thread_destroy(void* threadId)
{
	osThreadTerminate(threadId);
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
   (void)osDelay(ms);
}

void *HAL_Malloc(_IN_ uint32_t size)
{
	return pvPortMalloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    vPortFree(ptr);
}

void *HAL_MutexCreate(void)
{
	return (void *)osMutexNew(NULL);
}


void HAL_MutexDestroy(_IN_ void * mutex)
{
	osStatus ret;
	
    if(osOK != (ret = osMutexDelete((osMutexId)mutex)))
    {
		HAL_Printf("HAL_MutexDestroy err, err:%d\n\r",ret);
	}
}

void HAL_MutexLock(_IN_ void * mutex)
{
	osStatus ret;

	if(osOK != (ret = osMutexWait((osMutexId)mutex, osWaitForever)))
	{
		HAL_Printf("HAL_MutexLock err, err:%d\n\r",ret);
	}
}

void HAL_MutexUnlock(_IN_ void * mutex)
{
	osStatus ret;

	if(osOK != (ret = osMutexRelease((osMutexId)mutex)))
	{
		HAL_Printf("HAL_MutexUnlock err, err:%d\n\r",ret);
	}	
}
#else
void hal_thread_create(void** threadId, void (*fn)(void*), void* arg)
{

}

void HAL_SleepMs(_IN_ uint32_t ms)
{
	(void)HAL_Delay(ms);
}

void *HAL_Malloc(_IN_ uint32_t size)
{
	return malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
   free(ptr);
}

void *HAL_MutexCreate(void)
{
	return (void *)1;
}


void HAL_MutexDestroy(_IN_ void* mutex)
{
	return;
}

void HAL_MutexLock(_IN_ void* mutex)
{
	return;
}

void HAL_MutexUnlock(_IN_ void* mutex)
{
	return;
}

#endif


#ifdef __cplusplus
}
#endif
