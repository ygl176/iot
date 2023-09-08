/**
 * @file mqtt.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "log.h"
#include "mqtt.h"
#include <string.h>
#include "common.h"
#include "transport.h"
#include "MQTTConnect.h"
#include "MQTTFormat.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"

static uint16_t mqtt_flag;
static void *mqtt_lock = NULL;

void mqtt_flag_set(uint16_t flag)
{
    ATOMIC_SETH_BIT(mqtt_flag, flag);
}


void mqtt_flag_clear(uint16_t flag)
{
    ATOMIC_CLEARH_BIT(mqtt_flag, flag);
}


bool mqtt_flag_get(uint16_t flag)
{
    return (bool)(mqtt_flag & flag);
}

bool mqtt_init()
{
    response_t mqtt_resp = mqtt_get_resp();

    if(mqtt_resp->buf != NULL)
    {
        HAL_Free(mqtt_resp->buf);
        mqtt_resp->buf = NULL;
        mqtt_resp->buf_size = 0;
        mqtt_resp->buf_num = 0;
    }

    if(mqtt_lock != NULL)
    {
        Log_w("mqtt initialed");
        return false;
    }

    mqtt_lock = HAL_MutexCreate();

    if(mqtt_lock == NULL)
    {
        Log_e("mqtt lock creat err");
        return false;
    }

    return true;
}


bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive)
{
    bool ret = true;
    uint32_t target_time;
    response_t resp = mqtt_get_resp();

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = (char*)client_id;
    data.keepAliveInterval = keep_alive;
    data.cleansession = 1;
    data.username.cstring = DEVICE_NAME;
    data.password.cstring = DEVICE_KEY;

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);         //发送命令缓冲区申请

    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return false;
    }
    uint16_t len = MQTTSerialize_connect(mqtt_buff, MQTT_MAX_BUFF, &data);      //初始化报文

    HAL_MutexLock(mqtt_lock); //mqtt锁，指定 mqtt 响应报文接收缓冲区

    resp->buf = malloc(AT_BUFF_LEN);        //mqtt 接收缓冲区申请

    if(resp->buf == NULL)
    {
        Log_e("mqtt connect resp buf malloc failed");
        ret = false;
        goto EXIT;
    }
    resp->buf_size = AT_BUFF_LEN;

    mqtt_flag_clear(MQTT_CON_FLAG);     //清除目的标志位

    if(!transport_sendPacketBuffer(mqtt_buff, len, resp))       //发送报文
    {
        Log_e("connect msg send failed");
        ret = false;
        goto EXIT;
    }

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    target_time = HAL_GetTick() + MQTT_WAIT_TIME_MS;

    while(!mqtt_flag_get(MQTT_CON_FLAG) && HAL_GetTick() < target_time);  //规定时间内等待目的标志置位

    if(HAL_GetTick() >= target_time)
    {
        Log_e("mqtt connect timeout");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_CON_FLAG);


    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == CONNACK)     //解析响应报文
    {
        uint8_t sessionPresent,connack_rc;

        if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqtt_buff, MQTT_MAX_BUFF) != 1 || connack_rc != 0)
        {
            Log_e("mqtt connect ack failed");
            ret = false;
        }
    }
    else
        ret = false;

    EXIT: //程序错误退出

    HAL_Free(mqtt_buff);    //释放资源
    HAL_Free(resp->buf);

    resp->buf = NULL;

    HAL_MutexUnlock(mqtt_lock);

    return ret;
}


/**
 * @brief 断开连接
 * 
 */
void mqtt_disconnect()
{
    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);

    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return;
    }

    uint16_t len = MQTTSerialize_disconnect(mqtt_buff, MQTT_MAX_BUFF);

    if(!transport_sendPacketBuffer(mqtt_buff, len, NULL))
    {
        Log_e("disconnect mug send failed");
    }

    HAL_Free(mqtt_buff);

    return;
}

/**
 * @brief 订阅消息
 * 
 * @param sub_topic 
 * @param req_qos 
 * @param msgid 
 * @return true 
 * @return false 
 */
bool mqtt_subscribe(uint8_t* sub_topic, uint16_t req_qos, uint16_t msgid)
{
    bool ret = true;
    uint32_t target_time;

    response_t resp = mqtt_get_resp();

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);
    if(mqtt_buff == NULL)
    {
        Log_e("mqtt sub buf malloc failed");
        return false;
    }

    MQTTString topicName = MQTTString_initializer;
    topicName.cstring = (char*)sub_topic;
    uint16_t len = MQTTSerialize_subscribe(mqtt_buff, MQTT_MAX_BUFF, 0, msgid, 1, &topicName, &req_qos);

    HAL_MutexLock(mqtt_lock);

    resp->buf = HAL_Malloc(AT_BUFF_LEN);
    if(resp->buf == NULL)
    {
        Log_e("mqtt sub resp buf malloc failed");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_SUB_FLAG);

    if(!transport_sendPacketBuffer(mqtt_buff, len, resp))
    {
        Log_e("sub msg send failed");
        ret = false;
        goto EXIT;
    }

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    target_time = HAL_GetTick() + MQTT_WAIT_TIME_MS;

    while(!mqtt_flag_get(MQTT_SUB_FLAG) && HAL_GetTick() < target_time);  //规定时间内等待目的标志置位

    if(HAL_GetTick() >= target_time)
    {
        Log_e("mqtt sub timeout");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_SUB_FLAG);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == SUBACK)
    {
        uint16_t subid;
        uint16_t sub_count;
        uint8_t qos;

        uint8_t rc = MQTTDeserialize_suback(&subid, 1, &sub_count, (char*)&qos, mqtt_buff, MQTT_MAX_BUFF);

        if(!rc || subid != msgid || qos == 0x80)
            ret = false;
    }
    else
        ret = false;

    EXIT:

    HAL_Free(mqtt_buff);
    HAL_Free(resp->buf);
    resp->buf = NULL;

    HAL_MutexUnlock(mqtt_lock);

    return ret;
}


bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid)
{
    bool ret = true;
    uint32_t target_time;
    MQTTString topicName = MQTTString_initializer;
    tbsp_esp8266 p_esp = dev_esp_get();
    response_t resp = mqtt_get_resp();

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);
    if(mqtt_buff == NULL)
    {
        Log_e("mqtt sub buf malloc failed");
        return false;
    }

    uint16_t len = MQTTSerialize_unsubscribe(mqtt_buff, MQTT_MAX_BUFF, 0, msgid, 1, &topicName);

    HAL_MutexLock(p_esp->lock);

    resp->buf = HAL_Malloc(AT_BUFF_LEN);
    if(resp->buf == NULL)
    {
        Log_e("mqtt sub resp buf malloc failed");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_UNSUB_FLAG);

    if(!transport_sendPacketBuffer(mqtt_buff, len, resp))
    {
        Log_e("unsub msg send failed");
        ret = false;
        goto EXIT;
    }

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    target_time = HAL_GetTick() + MQTT_WAIT_TIME_MS;

    while(!mqtt_flag_get(MQTT_UNSUB_FLAG) && HAL_GetTick() < target_time);  //规定时间内等待目的标志置位

    if(HAL_GetTick() >= target_time)
    {
        Log_e("mqtt sub timeout");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_UNSUB_FLAG);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == UNSUBACK)
    {
        uint16_t unsub_id;

        uint8_t rc = MQTTDeserialize_unsuback(&unsub_id, mqtt_buff, MQTT_MAX_BUFF);

        if(!rc || unsub_id != msgid)
            ret = false;
    }
    else 
        ret = false;
    
    EXIT:

    HAL_Free(resp->buf);
    HAL_Free(mqtt_buff);
    resp->buf = NULL;

    HAL_MutexUnlock(p_esp->lock);

    return ret;
}


/**
 * @brief 发布消息 qs0
 *
 * @return true 
 * @return false 
 */
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload)
{
    bool ret;
    MQTTString topicName = MQTTString_initializer;
    response_t resp = mqtt_get_resp();

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);
    if(mqtt_buff == NULL)
    {
        Log_e("mqtt sub buf malloc failed");
        return false;
    }

    // resp->buf = HAL_Malloc(AT_BUFF_LEN);
    // if(resp->buf == NULL)
    // {
    //     Log_e("mqtt sub resp buf malloc failed");
    //     ret = false;
    //     goto EXIT;
    // }

    topicName.cstring = (char*)pub_topic;
    uint16_t len = MQTTSerialize_publish(mqtt_buff, MQTT_MAX_BUFF, 0 ,0 , 0, 0, topicName, payload, strlen((char*)payload));

    if(!transport_sendPacketBuffer(mqtt_buff, len, NULL))
    {
        Log_e("pub msg send failed");
        ret = false;
    }

//    EXIT:

    // HAL_Free(resp->buf);
    HAL_Free(mqtt_buff);
    resp->buf = NULL;

    return ret;
}



/**
 * @brief 发送心跳包
 * 
 * @return true 
 * @return false 
 */
bool mqtt_ping()
{
    bool ret = true;
    uint32_t target_time;
    response_t resp = mqtt_get_resp();

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);
    if(mqtt_buff == NULL)
    {
        Log_e("mqtt sub buf malloc failed");
        return false;
    }
    uint16_t len = MQTTSerialize_pingreq(mqtt_buff, MQTT_MAX_BUFF);

    HAL_MutexLock(mqtt_lock);
    
    resp->buf = HAL_Malloc(AT_BUFF_LEN);
    if(resp->buf == NULL)
    {
        Log_e("mqtt sub resp buf malloc failed");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_PING_FLAG);

    if(!transport_sendPacketBuffer(mqtt_buff, len, resp))
    {
        Log_e("pub msg send failed");
        ret = false;
        goto EXIT;
    }

    memset(mqtt_buff,0, MQTT_MAX_BUFF);

    target_time = HAL_GetTick() + MQTT_WAIT_TIME_MS;

    while(!mqtt_flag_get(MQTT_PING_FLAG) && HAL_GetTick() < target_time);  //规定时间内等待目的标志置位

    if(HAL_GetTick() >= target_time)
    {
        Log_e("mqtt sub timeout");
        ret = false;
        goto EXIT;
    }

    mqtt_flag_clear(MQTT_PING_FLAG);    

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) != PINGRESP)
    {
        ret = false;
    }

    EXIT:

    HAL_Free(resp->buf);
    HAL_Free(mqtt_buff);
    resp->buf = NULL;

    HAL_MutexUnlock(mqtt_lock);

    return ret;
}
