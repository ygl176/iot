/**
 * @file bsp_dh11.c
 * @author
 * @brief 
 * @version 0.1
 * @date
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bsp_dh11.h"
#include "bsp_delay.h"
#include "main.h"

#define DH11_START_MS           18
#define DH11_POWER_ON_MS        1000
//芯片响应最大等待时间
#define DH11_WAIT_RESP_MS       1000

#define DH11_PIN_UP         HAL_GPIO_WritePin(TEMP_GPIO_Port, TEMP_Pin, GPIO_PIN_SET)
#define DH11_PIN_DOWN       HAL_GPIO_WritePin(TEMP_GPIO_Port, TEMP_Pin, GPIO_PIN_RESET)
#define DH11_READ           HAL_GPIO_ReadPin(TEMP_GPIO_Port, TEMP_Pin)


//设置引脚位写状态
static void dh11_write_mode()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    DH11_PIN_UP;

    GPIO_InitStruct.Pin = TEMP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TEMP_GPIO_Port, &GPIO_InitStruct);
}


//设置引脚为读状态
static void dh11_read_mode()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = TEMP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TEMP_GPIO_Port, &GPIO_InitStruct);
}


/**
 * @brief 获取一位数据
 * 
 * @return true 
 * @return false 
 */
static uint8_t dh11_read_bit()
{
    uint8_t cnt = 0;

    while(DH11_READ && cnt < 100)   //等待低电平
    {
        cnt++;
        Delay_us(1);
    }

    cnt = 0;

    while((!DH11_READ) && cnt < 100)    //等待高电平
    {
        cnt++;
        Delay_us(1);
    }

    Delay_us(40);

    if(DH11_READ)   //检测数据位
        return 1;
    return 0;
}


/**
 * @brief 获取一字节数据
 * 
 * @return uint8_t 
 */
static uint8_t dh11_read_byte()
{
    uint8_t i, data = 0;

    for(i = 0; i < 8; ++i)
    {
        data <<= 1;
        data |= dh11_read_bit();
    }

    return data;
}


bool dh11_rst()
{
    uint8_t retry = 0;
    
    dh11_write_mode();
    DH11_PIN_DOWN;
    HAL_Delay(DH11_START_MS);
    DH11_PIN_UP;
    Delay_us(30);

    dh11_read_mode();

    while(DH11_READ && retry < 100) //等待低电平响应
    {
        retry++;
        Delay_us(1);
    }

    if(retry >= 100) 
        return false;
    retry = 0;

    while((!DH11_READ) && retry < 100) //等待高电平
    {
        retry++;
        Delay_us(1);
    }

    if(retry >= 100)
        return false;
    return true;
}


void dh11_init()
{

    HAL_Delay(DH11_POWER_ON_MS);
    dh11_rst();
}


bool dh11_get_data(float *temp, float *humi)
{
    uint8_t buf[5];

    if(!dh11_rst())
    {
        return false;
    }

    for(uint8_t i = 0; i < 5; ++i)
    {
        buf[i] = dh11_read_byte();
    }

    if((uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
    {
        return false;
    }
    
    *temp = (float)(buf[2] & 0x7f) + (buf[3] * 0.1);
    *humi = (float)(buf[0] + 0.1 * buf[1]);

    if(buf[2] & 0x80)
    	*temp *= -1;

    return true;
}
