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
#include "common.h"


bool ESP8266_Init(tbsp_esp8266 esp);

void ESP8266_Rst(tbsp_esp8266 esp)
{
    
}

bool ESP8266_Cmd(tbsp_esp8266 esp, char *cmd, uint32_t wait_ms)
{
    osMutexAcquire(esp->lock, portMAX_DELAY);

    

    while(esp->resp_notice != true)
    {

    }

    osMutexRelease(esp->lock);
}

bool ESP8266_AT_Test(tbsp_esp8266 esp);

bool ESP8266_Mode_Set(tbsp_esp8266 esp, eESP_Mode eMode);

bool ESP8266_JoinAP(tbsp_esp8266 esp, char *pSSID, char *pPassWord);

// bool ESP8266_DHCP_CUR(void);

// bool ESP8266_BuildAP(char *pSSID, char *pPassWord, eAP_PsdMode eMode);

bool ESP8266_Link_Server(tbsp_esp8266 esp, eNetPro eNet, char *ip, char *ComNum, eID_NO id);

eAP_Link_Sta ESP8266_GET_LinkStatus(tbsp_esp8266 esp);

uint8_t ESP8266_Get_IdLinkStatus(tbsp_esp8266 esp);

bool ESP8266_Inquire_ApIp(tbsp_esp8266 esp, char *pApIp, uint8_t buf_len);

bool ESP8266_UnvarnishSend(tbsp_esp8266 esp);

void ESP8266_ExitUnvarnishSend(tbsp_esp8266 esp);

bool ESP8266_SendStr(tbsp_esp8266 esp, char *pStr, uint32_t StrLen, eID_NO Id);

char* ESP8266_RecStr(tbsp_esp8266 esp);
