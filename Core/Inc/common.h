#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "config.h"

#ifdef OS_USED
#include "cmsis_os.h"
#endif

#define 	_IN_            /* 表明这是一个输入参数. */
#define		_OU_            /* 表明这是一个输出参数. */

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

//uint16_t 类型置位、清除
#define SET_EVENT(event, arg)               ATOMIC_SETH_BIT((event), (arg))
#define CLEAR_EVENT(event, arg)             ATOMIC_CLEARH_BIT((event), (arg))
#define GET_EVEVT(event, arg)               ((event) & (arg))

/**
 * @brief 指定串口输出字符串
 * 
 * @param USARTx 串口
 * @param Data 数据
 * @param ... 
 */
void usart_printf(UART_HandleTypeDef *USARTx, const char *fmt, ...);

/**
 * @brief 微秒级阻塞延迟
 * 
 * @param count 延时计数
 */
void delay_us(uint32_t count);

/**
 * @brief 毫秒级阻塞延迟
 * 
 * @param count 延时计数
 */
void delay_ms(uint32_t count);

#ifdef OS_USED
typedef void * osThreadId;
void hal_thread_create(volatile void* threadId, uint16_t stackSize, int Priority, void (*fn)(void*), void* arg);
void hal_thread_destroy(void* threadId);
void HAL_SleepMs(_IN_ uint32_t ms);

#ifdef MUTEX
void *HAL_MutexCreate(void);
void HAL_MutexDestroy(_IN_ void* mutex);
void HAL_MutexLock(_IN_ void* mutex);
void HAL_MutexUnlock(_IN_ void* mutex);
#endif  //MUTEX

#ifdef QUEUE
void *HAL_QueueCreate(uint32_t QueLen, uint32_t ItemSize);
#endif  //QUEUE

#ifdef COUNT_SEM
void *HAL_SemaphoreCreate(_IN_ uint32_t max_count, _IN_ uint32_t init_count);
bool HAL_SemaphoreWait(_IN_ void* semaphore, uint32_t wait_ms);
void HAL_SemaphoreRelease(_IN_ void* semaphore);
#endif

void *HAL_Malloc(_IN_ uint32_t size);
void HAL_Free(_IN_ void *ptr);
#endif  //OS_USED

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_H */
