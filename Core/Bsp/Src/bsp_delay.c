/**
 * @file bsp_delay.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bsp_delay.h"
#include "main.h"
#include "stdbool.h"
//#include "tim.h"

//��ȷ��ʱͨ����ⶨʱ��Ӳ����־λʵ�֣�����߳�ͬʱ���ûᵼ����ʱ�쳣
static bool delay_lock = false;

#if USE_SYSTICK

static uint32_t fac_us;

void Delay_init()
{
    //ʱ��Ϊ HCLK 8��Ƶ��72M��Ƶʱ����ʱ��ʱ��Ϊ 9M
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);

    fac_us = HAL_RCC_GetHCLKFreq() / 8;
}

//�Ǿ�ȷ��ʱ
void Delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; ++i)
        Delay_us(1000);
}


void Delay_us(uint32_t us)
{
    uint32_t temp;

    if(delay_lock)
    {
        #ifdef LOG
        log_e("Delay_ms is using");
        #endif
        return;
    }
    delay_lock = true;

    SysTick->LOAD = (uint32_t)us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;

    do
    {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));
    
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;

    delay_lock = false;
}

#else

TIM_HandleTypeDef htim6;

//ʹ�� TIM6 ��Ϊ��ʱʱ��Դ
void Delay_init()
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 71;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = UINT16_MAX;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_Base_Init(&htim6) != HAL_OK) //��ʱ��ʱ������1us
    {
        Error_Handler();
    }
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* TIM6 clock enable */
    __HAL_RCC_TIM6_CLK_ENABLE();
  /* USER CODE BEGIN TIM6_MspInit 1 */

  /* USER CODE END TIM6_MspInit 1 */
  }
}

void Delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; ++i)
    {
        Delay_us(1000);
    }
}

void Delay_us(uint16_t us)
{
    if(delay_lock)
    {
        #ifdef LOG
        log_e("Delay_ms is using");
        #endif
        return;
    }
    delay_lock = true;

    __HAL_TIM_SET_AUTORELOAD(&htim6, us);

    HAL_TIM_Base_Start(&htim6);
    
    while(__HAL_TIM_GET_FLAG(&htim6, TIM_FLAG_UPDATE) == RESET);
    __HAL_TIM_CLEAR_FLAG(&htim6,TIM_FLAG_UPDATE);
    
    HAL_TIM_Base_Stop(&htim6);

    delay_lock = false;
}

#endif
