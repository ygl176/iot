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

#include "mqtt.h"
#include <string.h>
#include "transport.h"
#include "MQTTConnect.h"
#include "MQTTFormat.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"


// const uint8_t client_id[] = CLIENT_ID;
// const uint8_t device_name[] = DEVICE_NAME;
// const uint8_t device_key[] = DEVICE_KEY;
// const uint8_t mqtt_server[] = MQTT_SERVER;
// const uint8_t sub_topic[] = SUB_TOPIC;
// const uint8_t pub_topic[] = PUB_TOPIC;
uint8_t mqtt_buff[MQTT_MAX_BUFF];

bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = client_id;
    data.keepAliveInterval = keep_alive;
    data.cleansession = 1;
    data.username.cstring = DEVICE_NAME;
    data.password.cstring = DEVICE_KEY;

    uint16_t len = MQTTSerialize_connect(mqtt_buff, MQTT_MAX_BUFF, &data);

    transport_sendPacketBuffer(mqtt_buff, len);

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == CONNACK)
    {
        uint8_t sessionPresent,connack_rc;

        if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqtt_buff, MQTT_MAX_BUFF) != 1 || connack_rc != 0)
        {
            return false;
        }
    }
    else
        return false;

    return true;
}


/**
 * @brief 断开连接
 * 
 */
void mqtt_disconnect()
{
    uint16_t len = MQTTSerialize_disconnect(mqtt_buff, MQTT_MAX_BUFF);

    transport_sendPacketBuffer(mqtt_buff, len);

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);
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

        if(!rc || subid != msgid || qos == 0x80) return false;
    }
    else return false;

    return true;
}


bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid)
{
    MQTTString topicName = MQTTString_initializer;

    uint16_t len = MQTTSerialize_unsubscribe(mqtt_buff, MQTT_MAX_BUFF, 0, msgid, 1, &topicName);

    transport_sendPacketBuffer(mqtt_buff, MQTT_MAX_BUFF);

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) == UNSUBACK)
    {
        uint16_t unsub_id;

        uint8_t rc = MQTTDeserialize_unsuback(&unsub_id, mqtt_buff, MQTT_MAX_BUFF);

        if(!rc || unsub_id != msgid) return false;
    }
    else return false;

    return true;
}


/**
 * @brief 发布消息
 * 
 * @return true 
 * @return false 
 */
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload)
{
    MQTTString topicName = MQTTString_initializer;

    topicName.cstring = pub_topic;

    uint16_t len = MQTTSerialize_publish(mqtt_buff, MQTT_MAX_BUFF, 0 ,0 , 0, 0, topicName, payload, strlen(payload));

    transport_sendPacketBuffer(mqtt_buff, len);

    memset(mqtt_buff, 0, MQTT_MAX_BUFF);

    return true;
}



/**
 * @brief 发送心跳包
 * 
 * @return true 
 * @return false 
 */
bool mqtt_ping()
{
    uint16_t len = MQTTSerialize_pingreq(mqtt_buff, MQTT_MAX_BUFF);

    transport_sendPacketBuffer(mqtt_buff, len);

    memset(mqtt_buff,0, MQTT_MAX_BUFF);

    if(MQTTPacket_read(mqtt_buff, MQTT_MAX_BUFF, transport_getdata) != PINGRESP)
    {
        return false;
    }

    return true;
}
