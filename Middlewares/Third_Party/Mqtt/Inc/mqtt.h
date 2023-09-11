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

#include "main.h"
#include <stdbool.h>
#include "bsp_at_esp8266.h"

#define CLIENT_ID       (char*)"VOIXMXT1OKesp"
#define DEVICE_NAME     (char*)"VOIXMXT1OKesp;12010126;9dab2;1702137600"
#define DEVICE_KEY      (char*)"42866c6458d133519e19bc5cab65441bd5e3303d5c1638a7ffb559acf522cd15;hmacsha256"

#define MQTT_SERVER     "VOIXMXT1OK.iotcloud.tencentdevices.com"
#define MQTT_PORT       "1883"

#define SUB_TOPIC       "$thing/down/property/VOIXMXT1OK/esp"
#define PUB_TOPIC       "$thing/up/property/VOIXMXT1OK/esp"

#define MQTT_MAX_BUFF       512

#define MQTT_WAIT_TIME_MS   5000

#define  MQTT_CON_FLAG 			(1<<2)
#define  MQTT_SUB_FLAG			(1<<9)
#define  MQTT_UNSUB_FLAG		(1<<11)
#define  MQTT_PING_FLAG 		(1<<13)

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
