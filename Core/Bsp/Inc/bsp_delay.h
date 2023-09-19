/**
 * @file bsp_delay.h
 * @author 
 * @brief ͨ��systick �� ������ʱ��ʵ�־�ȷ��ʱ
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __BSP_DELAY_H__
#define __BSP_DELAY_H__

#include "stm32f1xx_hal.h"

//�Ƿ�ʹ�� systick ʱ��Դ���о�ȷ��ʱ, systick ��Ϊʱ��Դʱ��hal�ⲻ������ systick ��Ϊʱ��
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