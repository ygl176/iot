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
#include "bsp_at_esp8266.h"
#include "MQTTUnsubscribe.h"


// const uint8_t client_id[] = CLIENT_ID;
// const uint8_t device_name[] = DEVICE_NAME;
// const uint8_t device_key[] = DEVICE_KEY;
// const uint8_t mqtt_server[] = MQTT_SERVER;
// const uint8_t sub_topic[] = SUB_TOPIC;
// const uint8_t pub_topic[] = PUB_TOPIC;

bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive)
{
    bool ret = true;
    tbsp_esp8266 p_esp = dev_esp_get();
    response_t resp = mqtt_get_resp();

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = client_id;
    data.keepAliveInterval = keep_alive;
    data.cleansession = 1;
    data.username.cstring = DEVICE_NAME;
    data.password.cstring = DEVICE_KEY;

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);     

    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");        //���������������
        return false;
    }

    HAL_MutexLock(p_esp->lock); //�������������޸����������Ϣ

    resp->buf = malloc(AT_BUFF_LEN);        //mqtt ���ջ���������

    if(resp->buf == NULL)
    {
        Log_e("mqtt connect resp buf malloc failed");
        ret = false;
        goto EXIT;
    }
    resp->buf_size = AT_BUFF_LEN;
    p_esp->mqtt_req_type = CONNACK;     //���õȴ���������

    uint16_t len = MQTTSerialize_connect(mqtt_buff, MQTT_MAX_BUFF, &data);      //��ʼ������

    if(!transport_sendPacketBuffer(mqtt_buff, len, resp, MQTT_WAIT_TIME_MS))    //���ͱ��Ĳ��ȴ���Ӧ
    {
        Log_e("connect msg send failed");
        ret = false;
        goto EXIT;
    }

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == CONNACK)     //������Ӧ����
    {
        uint8_t sessionPresent,connack_rc;

        if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqtt_buff, MQTT_MAX_BUFF) != 1 || connack_rc != 0)
        {
            ret = false;
        }
    }
    else
        ret = false;

    EXIT: //��������˳�

    HAL_Free(mqtt_buff);    //�ͷ���Դ
    HAL_Free(resp->buf);

    resp->buf = NULL;
    p_esp->mqtt_req_type = 0;
    HAL_MutexUnlock(p_esp->lock);

    return ret;
}


/**
 * @brief �Ͽ�����
 * 
 */
void mqtt_disconnect()
{
    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);

    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return false;
    }

    uint16_t len = MQTTSerialize_disconnect(mqtt_buff, MQTT_MAX_BUFF);

    if(!transport_sendPacketBuffer(mqtt_buff, len, NULL, 0))    //������Ӧ����
    {
        Log_e("disconnect msg send failed");
        return;
    }

    HAL_Free(mqtt_buff);
}

/**
 * @brief ������Ϣ
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
    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);

    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return false;
    }

    MQTTString topicName = MQTTString_initializer;

    topicName.cstring = sub_topic;

    uint16_t len = MQTTSerialize_subscribe(mqtt_buff, MQTT_MAX_BUFF, 0, msgid, 1, &topicName, &req_qos);

    transport_sendPacketBuffer(mqtt_buff, MQTT_MAX_BUFF);

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == SUBACK)
    {
        uint16_t subid;
        uint16_t sub_count;
        uint8_t qos;

        uint8_t rc = MQTTDeserialize_suback(&subid, 1, &sub_count, &qos, mqtt_buff, MQTT_MAX_BUFF);

        if(!rc || subid != msgid || qos == 0x80)
            ret = false;
    }
    else
        ret = false;

    HAL_Free(mqtt_buff);
    return ret;
}


bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid)
{
    bool ret = true;
    MQTTString topicName = MQTTString_initializer;

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);
    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return false;
    }

    uint16_t len = MQTTSerialize_unsubscribe(mqtt_buff, MQTT_MAX_BUFF, 0, msgid, 1, &topicName);

    transport_sendPacketBuffer(mqtt_buff, MQTT_MAX_BUFF);

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == UNSUBACK)
    {
        uint16_t unsub_id;

        uint8_t rc = MQTTDeserialize_unsuback(&unsub_id, mqtt_buff, MQTT_MAX_BUFF);

        if(!rc || unsub_id != msgid)
            ret = false;
    }
    else 
        ret = false;

    HAL_Free(mqtt_buff);
    return ret;
}


/**
 * @brief ������Ϣ
 * 
 * @return true 
 * @return false 
 */
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload)
{
    MQTTString topicName = MQTTString_initializer;

    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);
    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return false;
    }

    topicName.cstring = pub_topic;

    uint16_t len = MQTTSerialize_publish(mqtt_buff, MQTT_MAX_BUFF, 0 ,0 , 0, 0, topicName, payload, strlen(payload));

    transport_sendPacketBuffer(mqtt_buff, len);

    HAL_Free(mqtt_buff);
    return true;
}



/**
 * @brief ����������
 * 
 * @return true 
 * @return false 
 */
bool mqtt_ping()
{
    bool ret = true;
    uint8_t *mqtt_buff = HAL_Malloc(MQTT_MAX_BUFF);

    if(mqtt_buff == NULL)
    {
        Log_e("mqtt connect buf malloc failed");
        return false;
    }

    uint16_t len = MQTTSerialize_pingreq(mqtt_buff, MQTT_MAX_BUFF);

    transport_sendPacketBuffer(mqtt_buff, len);

    memset(mqtt_buff,0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) != PINGRESP)
    {
        ret = false;
    }

    HAL_Free(mqtt_buff);
    return ret;
}
