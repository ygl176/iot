/**
 * @file mqtt.h
 * @author 
 * @brief 
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __MQTT_H__
#define __MQTT_H__

#include "stm32f1xx_hal.h"
#include "export_err.h"
#include <stdbool.h>
#include "bsp_at_esp8266.h"

/* 云端保留主题的最大长度 */
#define MAX_SIZE_OF_CLOUD_TOPIC            ((MAX_SIZE_OF_DEVICE_NAME) + (MAX_SIZE_OF_PRODUCT_ID) + 64)

#define CLIENT_ID       (char*)"VOIXMXT1OKesp"
#define DEVICE_NAME     (char*)"VOIXMXT1OKesp;12010126;d0ff9;1702396800"
#define DEVICE_KEY      (char*)"741ef9bd25d225a15a8ccad941ce64fabe55d27f71d09003a02b7d1dd6cffa06;hmacsha256"

#define MQTT_SERVER     "VOIXMXT1OK.iotcloud.tencentdevices.com"
#define MQTT_PORT       "1883"

#define SUB_TOPIC       "$thing/down/property/VOIXMXT1OK/esp"
#define PUB_TOPIC       "$thing/up/property/VOIXMXT1OK/esp"

#define MQTT_MAX_BUFF       512

//mqtt报文超时时间
#define MQTT_WAIT_TIME_MS   5000

//mqtt接收响应报文标志定义
#define  MQTT_CON_FLAG 			(1<<2)
#define  MQTT_SUB_FLAG			(1<<9)
#define  MQTT_UNSUB_FLAG		(1<<11)
#define  MQTT_PING_FLAG 		(1<<13)

/**
 * @brief 服务质量等级
 *
 * 服务质量等级表示PUBLISH消息分发的质量等级
 */

void mqtt_flag_set(uint16_t flag);
void mqtt_flag_clear(uint16_t flag);
bool mqtt_flag_get(uint16_t flag);

bool mqtt_init();
bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive);
void mqtt_disconnect();
bool mqtt_subscribe(uint8_t* sub_topic, uint16_t req_qos, uint16_t msgid);
bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid);
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload);
bool mqtt_ping();

#endif //__MQTT_H__
