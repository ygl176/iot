/**
 * @file bsp_at_esp8266.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bsp_at_esp8266.h"
#include "ringbuff.h" 
#include "common.h"
#include "log.h"

sRingbuff esp_ring_buff;

static bsp_esp8266 esp;


//接收消息队列长度
#define RECV_QUEUE_LEN      4



response_t resp_malloc(uint32_t buf_len)
{
    response_t resp = HAL_Malloc(sizeof(response_t));
    if(resp == NULL)
    {
        Log_e("resp malloc err");
        return NULL;
    }

    resp->buf = HAL_Malloc(buf_len);
    if(resp->buf == NULL)
    {
        Log_e("resp buf malloc err");
        HAL_Free(resp);
        return NULL;
    }

    return resp;
}


void resp_release(response_t resp)
{
    if(resp && resp->buf)
        HAL_Free(resp->buf);

    if(resp)
    {
        HAL_Free(resp);
        resp = NULL;
    }
}


tbsp_esp8266 dev_esp_get()
{
    return &esp;
}

bool dev_esp_init(tbsp_esp8266 p_esp)
{
    p_esp->status = ESP8266_UNINITIALIZED;

    p_esp->lock = HAL_MutexCreate();
    if(p_esp->lock == NULL)
    {
        Log_e("esp create lock err");
        return false;
    }

    char *ring_buff = HAL_Malloc(RING_BUFF_LEN);

    if(ring_buff == NULL)
    {
        Log_e("malloc ringbuff err");
        return false;
    }

    p_esp->recv_que = HAL_QueueCreate(8, sizeof(response_t));
    if(p_esp->recv_que == NULL)
    {
        Log_e("esp create recv err");
        return false;
    }

    if(p_esp->resp)
    {
        resp_release(p_esp->resp);
        p_esp->resp = NULL;
    }
    p_esp->resp_notice = false;

    return true;
}



bool ESP8266_Init()
{
    tbsp_esp8266 p_esp = dev_esp_get();

    if(p_esp->status == ESP8266_INITIALIZED)
    {
        Log_e("esp is INITIALIZED");
    }

    dev_esp_init(p_esp);
}

void ESP8266_Rst()
{
    
}

response ESP8266_Cmd(response_t resp, char *cmd, uint32_t wait_ms)
{
    tbsp_esp8266 p_esp = dev_esp_get();
    uint32_t target_time = HAL_GetTick() + wait_ms;

    HAL_MutexLock(p_esp->lock);

    while(p_esp->resp_notice != true && HAL_GetTick() <= target_time)
    {
        // HAL_Delay(10);
    }

    if(HAL_GetTick() > target_time)
    {

    }

    HAL_MutexLock(p_esp->lock);
}

bool ESP8266_AT_Test(tbsp_esp8266 p_esp)
{

}

bool ESP8266_Mode_Set(eESP_Mode eMode);

bool ESP8266_JoinAP(char *pSSID, char *pPassWord);

// bool ESP8266_DHCP_CUR(void);

// bool ESP8266_BuildAP(char *pSSID, char *pPassWord, eAP_PsdMode eMode);

bool ESP8266_Link_Server(eNetPro eNet, char *ip, char *ComNum, eID_NO id);

eAP_Link_Sta ESP8266_GET_LinkStatus();

uint8_t ESP8266_Get_IdLinkStatus();

bool ESP8266_Inquire_ApIp(char *pApIp, uint8_t buf_len);

bool ESP8266_UnvarnishSend();

void ESP8266_ExitUnvarnishSend();

bool ESP8266_SendStr(char *pStr, uint32_t StrLen, eID_NO Id);

char* ESP8266_RecStr();
