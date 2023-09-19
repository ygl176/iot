/**
 * @file bsp_delay.h
 * @author 
 * @brief 通过systick 或 其他定时器实现精确延时
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __BSP_DELAY_H__
#define __BSP_DELAY_H__

#include "stm32f1xx_hal.h"

//是否使用 systick 时钟源进行精确延时, systick 作为时钟源时，hal库不能配置 systick 作为时基
#define USE_SYSTICK     0

void Delay_init();

/**
 * @brief 
 * 
 * @param ms 
 */
void Delay_ms(uint32_t ms);

#if USE_SYSTICK
void Delay_us(uint32_t us);
#else
void Delay_us(uint16_t us);
#endif


#endif //__BSP_DELAY_H__