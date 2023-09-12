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

#include <string.h>
#include "dev_info.h"
#include "bsp_at_esp8266.h"
#include "mqtt.h"
#include "common.h"
#include "log.h"

extern UART_HandleTypeDef huart1;
extern void IOT_Template_Message_Arrived_CallBack(const char *data, int size);

#define ESP_UART    (&huart1)
#define AT_WAIT_MS  5000
#define WIFI_ID			"Redmi K40"
#define WIFI_PASS		"qweasd4848"

sRingbuff esp_ring_buff;
static bsp_esp8266 esp;



//接收消息队列长度
// #define RECV_QUEUE_LEN      4
#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"


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
    ring_buff_init(&esp_ring_buff, ring_buff, RING_BUFF_LEN);

    p_esp->sema_rx = HAL_SemaphoreCreate(RING_BUFF_LEN, 0);
    if(p_esp->sema_rx == NULL)
    {
        Log_e("esp create sema err");
        return false;
    }

    p_esp->recv = HAL_Malloc(AT_CMD_MAX_LEN);
    if(p_esp->recv == NULL)
    {
        Log_e("esp create recv err");
        return false;
    }

    if(p_esp->resp)
    {
        resp_release(p_esp->resp);
        p_esp->resp = NULL;
    }

    p_esp->p_ring_buff = &esp_ring_buff;
    p_esp->recv_len = AT_CMD_MAX_LEN;
    p_esp->cur_recv_len = 0;
    p_esp->resp = NULL;
    p_esp->resp_notice = false;
    p_esp->recv_notice = false;

    return true;
}



bool ESP8266_Init()
{
    tbsp_esp8266 p_esp = dev_esp_get();

    if(p_esp->status == ESP8266_INITIALIZED)
    {
        Log_e("esp is INITIALIZED");
    }

    if(!dev_esp_init(p_esp))
    {
        Log_e("p_esp init failed");
        return false;
    }

    osThreadId_t esp_parse_id = NULL;

    hal_thread_create(esp_parse_id, 1024, osPriorityNormal, esp_parse, p_esp);

    if(!ESP8266_AT_Test())
    {
        return false;
    }

    if(!ESP8266_ATE(0))
    {
        return false;
    }

    if(!ESP8266_Mode_Set(STA))
    {
        return false;
    }

    if(!ESP8266_JoinAP(WIFI_ID, WIFI_PASS))
    {
        return false;
    }

    if(!ESP8266_Link_Server("TCP", MQTT_SERVER, MQTT_PORT, Single_ID_0))
    {
        return false;
    }

    if(!ESP8266_UnvarnishSend())
    {
        return false;
    }

    p_esp->status = ESP8266_INITIALIZED;

    return true;
}

void ESP8266_Rst()
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return ;
    }

    if(!ESP8266_Cmd(resp, AT_WAIT_MS, "AT+RST\r\n"))
    {
        Log_e("at reset failed");
    }

    resp_release(resp);
}

bool ESP8266_Cmd(response_t resp, uint32_t wait_ms, const char *fmt, ...)
{
    bool ret = true;
    tbsp_esp8266 p_esp = dev_esp_get();
    uint32_t target_time = HAL_GetTick() + wait_ms;

    char *cmd = HAL_Malloc(128);
    if(cmd == NULL)
    {
        Log_e("esp cmd malloc failed");
    }

    va_list ap;
    uint32_t count;

    va_start(ap, fmt);
	count = vsnprintf(cmd, 128, fmt, ap);
	va_end(ap);

    HAL_MutexLock(p_esp->lock);

    p_esp->resp = resp;
    p_esp->resp_notice = false;

    //发送命令
    HAL_UART_Transmit(ESP_UART, (uint8_t*)cmd, count, HAL_MAX_DELAY);

    while(p_esp->resp_notice != true && HAL_GetTick() <= target_time)
    {
        // HAL_Delay(10);
    }

    if(HAL_GetTick() > target_time) //超时
    {
        Log_e("esp cmd(%s) timeout %dms", cmd, wait_ms);

        p_esp->resp_status = false;
        ret = false;
    }

    if(p_esp->resp_notice)
    {
        if(!p_esp->resp_status) //结果错误
        {
            Log_e("esp cmd(%s) failed", cmd);
            ret = false;
        }
    }

    p_esp->resp = NULL;
    p_esp->resp_notice = false;

    HAL_Free(cmd);

    HAL_MutexUnlock(p_esp->lock);

    return ret;
}

bool ESP8266_AT_Test()
{
    uint8_t count = 0;
    response_t resp = resp_malloc(ESP_BUFF_LEN);

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    Log_i("at test...");

    while(count < 10)
    {
        delay_ms(1000);
        
        Log_i("at test count:%d...", count);

        if(ESP8266_Cmd(resp, AT_WAIT_MS, "AT\r\n"))
        {
            Log_i("at test success");
            resp_release(resp);

            return true;
        }
        ++count;
    }

    resp_release(resp);

    Log_e("at test failed");

    return false;
}

bool ESP8266_ATE(uint8_t mode)
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);
    bool ret = true;

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(!ESP8266_Cmd(resp, AT_WAIT_MS, "ATE%d\r\n", mode))
    {
        Log_e("echo set err");
        ret = false;
    }

    resp_release(resp);

    return ret;
}

bool ESP8266_Mode_Set(eESP_Mode eMode)
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);
    bool ret = true;

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(!ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CWMODE_CUR=%d\r\n", eMode))
    {
        Log_e("esp mode set err");
        ret = false;
    }

    resp_release(resp);

    return ret;
}

bool ESP8266_JoinAP(char *pSSID, char *pPassWord)
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);
    bool ret = true;

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(!ESP8266_Cmd(resp, 20000, "AT+CWJAP=\"%s\",\"%s\"\r\n", pSSID, pPassWord))
    {
        Log_e("Join ap failed");
        ret = false;
    }

    resp_release(resp);

    return ret;

}

// bool ESP8266_DHCP_CUR(void);

// bool ESP8266_BuildAP(char *pSSID, char *pPassWord, eAP_PsdMode eMode);

bool ESP8266_Link_Server(char* net, char *ip, char *ComNum, eID_NO id)
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);
    bool ret = true;

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(id < 5)
    {
        if(!ESP8266_Cmd(resp, 10000, "AT+CIPSTART=%d,\"%s\",\"%s\",%s\r\n", id, net, ip, ComNum))
        {
            Log_e("server link failed");
            ret = false;
        }
    }
    else
    {
        if(!ESP8266_Cmd(resp, 10000, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", net, ip, ComNum))
        {
            Log_e("server link failed");
            ret = false;
        }
    }

    resp_release(resp);

    return ret;
}

eAP_Link_Sta ESP8266_GET_LinkStatus()
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIPSTATUS\r\n"))
    {
        if (strstr(resp->buf, "STATUS:2"))
			return 2;
		
		else if(strstr(resp->buf, "STATUS:3"))
			return 3;
		
		else if(strstr(resp->buf, "STATUS:4"))
			return 4;		

    }
    else
    {
        Log_e("esp get link status failed");
    }


    resp_release(resp);

    return 0;
}

uint16_t ESP8266_Get_IdLinkStatus()
{
    uint16_t ucIdLinkStatus = 0x00;
    response_t resp = resp_malloc(ESP_BUFF_LEN);

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIPSTATUS\r\n"))
    {
        if (strstr(resp->buf, "CIPSTATUS:0"))
			SET_EVENT(ucIdLinkStatus, 0x01);
        else
            CLEAR_EVENT(ucIdLinkStatus, 0x01);
		
		if (strstr(resp->buf, "CIPSTATUS:1"))
			SET_EVENT(ucIdLinkStatus, 0x02);
        else
            CLEAR_EVENT(ucIdLinkStatus, 0x02);
        
        if (strstr(resp->buf, "CIPSTATUS:2"))
			SET_EVENT(ucIdLinkStatus, 0x04);
        else
            CLEAR_EVENT(ucIdLinkStatus, 0x04);
        
        if (strstr(resp->buf, "CIPSTATUS:3"))
			SET_EVENT(ucIdLinkStatus, 0x08);
        else
            CLEAR_EVENT(ucIdLinkStatus, 0x08);
        
        if (strstr(resp->buf, "CIPSTATUS:4"))
			SET_EVENT(ucIdLinkStatus, 0x10);
        else
            CLEAR_EVENT(ucIdLinkStatus, 0x10);

    }
    else
    {
        Log_e("esp get id link status failed");
    }


    resp_release(resp);

    return ucIdLinkStatus;

}

bool ESP8266_Inquire_ApIp(char *pApIp, uint8_t buf_len)
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);
    bool ret = true;
    char *ptr;

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIFSR\r\n"))
    {
        ptr = strstr(resp->buf, "APIP,\"");

        if(ptr)
        {
            ptr += 6;

            for(uint8_t i = 0; i < buf_len; ++i, ++ptr)
            {
                pApIp[i] = *ptr;

                if(pApIp[i] == '\"')
                {
                    pApIp = '\0';
                    break;
                }
            }
        }

    }
    else
    {
        Log_e("esp get link status failed");
        ret = false;
    }


    resp_release(resp);

    return ret;
}

bool ESP8266_UnvarnishSend()
{
    response_t resp = resp_malloc(ESP_BUFF_LEN);
    bool ret = true;

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(!ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIPMODE=1\r\n"))
    {
        Log_e("esp unvarnish err");
        ret = false;
    }

    if(!ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIPSEND\r\n"))
    {
        Log_e("esp unvarnish err");
        ret = false;
    }

    resp_release(resp);

    return ret;
}

void ESP8266_ExitUnvarnishSend()
{
    delay_ms(1000);

    HAL_UART_Transmit(ESP_UART, (uint8_t*)"+++", 3, HAL_MAX_DELAY);

    delay_ms(500);
}

bool ESP8266_SendStr(char *pStr, uint32_t StrLen, eID_NO Id)
{
    bool ret = true;
    tbsp_esp8266 p_esp = dev_esp_get();
    response_t resp = resp_malloc(ESP_BUFF_LEN);

    if(resp == NULL)
    {
        Log_e("at test failed: malloc resp");
        return false;
    }

    if(p_esp->status == ESP8266_INITIALIZED)
    {
        HAL_UART_Transmit(ESP_UART, (uint8_t*)pStr, StrLen, HAL_MAX_DELAY);
    }
    else if(Id < 5)
    {
        if(ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIPSEND=%d,%d", Id, StrLen + 2))
        {
            if(!ESP8266_Cmd(resp, AT_WAIT_MS, "%s\r\n", pStr))
            {
                Log_e("esp end str err");
                ret = false;
            }
        }
        else
        {
            Log_e("esp send cmd err");
            ret = false;
        }
    }
    else
    {
        if(ESP8266_Cmd(resp, AT_WAIT_MS, "AT+CIPSEND=%d", StrLen + 2))
        {
            if(!ESP8266_Cmd(resp, AT_WAIT_MS, "%s\r\n", pStr))
            {
                Log_e("esp end str err");
                ret = false;
            }
        }
        else
        {
            Log_e("esp send cmd err");
            ret = false;
        }
    }

    resp_release(resp);

    return ret;
}

char* ESP8266_RecStr();

static bool esp_recv_ReadMessage(tbsp_esp8266 p_esp)
{
    uint32_t read_len = 0;
    uint8_t ch = 0;
    bool is_full = false;
    memset(p_esp->recv, 0x00, p_esp->recv_len);
    p_esp->cur_recv_len = 0;

    while(1)
    {
        if(!HAL_SemaphoreWait(p_esp->sema_rx, 5000))
        {
            return false;
        }
        ring_buff_pop_data(p_esp->p_ring_buff, &ch, 1);

        if(read_len < p_esp->recv_len)
        {
            p_esp->recv[read_len++] = ch;
            p_esp->cur_recv_len = read_len;
        }
        else
        {
            is_full = true;
        }

        if(p_esp->recv_notice && ch == '\0' && p_esp->recv[read_len - 2] == '\n')
        {
            p_esp->recv_notice = false;
            if(is_full)
            {
                Log_e("read message failed. data out of buffer size(%d)", p_esp->recv_len);
                memset(p_esp->recv, 0x00, p_esp->recv_len);
                p_esp->cur_recv_len = 0;
                ring_buff_flush(p_esp->p_ring_buff);
                return false;
            }
            return true;
        }
    }
}

void esp_parse(void *arg)
{
    tbsp_esp8266 p_esp = dev_esp_get();
    uint8_t recv_type;

    Log_d("esp_parse start...");

    while(1)
    {
        if(esp_recv_ReadMessage(p_esp))
        {
#ifdef PRINT_CMD

#endif
            if(p_esp->status == ESP8266_INITIALIZED)    //已连接服务器，开启透传
            {
                recv_type = (p_esp->recv[0]) >> 4;
                //待替换非主动请求检测
                if(recv_type == 3)//接收报文为publish
                {
                    IOT_Template_Message_Arrived_CallBack(p_esp->recv, p_esp->cur_recv_len);
                }
                else if(p_esp->resp != NULL) //mqtt主动请求
                {
                    if(recv_type == 2 || recv_type == 9 || recv_type == 11 || recv_type == 13)
                    {
                        memcpy(p_esp->resp->buf, p_esp->recv, p_esp->cur_recv_len);
                        p_esp->resp->buf_num = p_esp->cur_recv_len;
                        
                        //设置对应标志位
                        mqtt_flag_set(1 << recv_type);
                    }
                }
            }
            else    //ESP 还未初始化,分析AT指令响应
            {
                if(p_esp->resp != NULL)
                {
                    if(strstr(p_esp->recv, AT_RESP_END_OK) || strstr(p_esp->recv, ">"))
                    {
                        p_esp->resp_status = true;
                        memcpy(p_esp->resp->buf, p_esp->recv, p_esp->cur_recv_len);
                    }
                    else if(strstr(p_esp->recv, AT_RESP_END_ERROR) || strstr(p_esp->recv, AT_RESP_END_FAIL))
                    {
                        p_esp->resp_status = false;
                    }
                    else
                    {
                        continue;
                    }
    
                    p_esp->resp = NULL;
                    p_esp->resp_notice = true;
                }
            }

        }
        else    //获取消息失败
        {

        }
    }

}
