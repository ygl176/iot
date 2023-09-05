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

#define CLIENT_ID       (uint8_t*)"PM62I41E39ESP8266"
#define DEVICE_NAME     (uint8_t*)"PM62I41E39ESP8266;12010126;DXLQO;1692090392"
#define DEVICE_KEY      (uint8_t*)"9889a494cb5556eb869a2c6b4573a5cd31461dd1a099e217f450973afb82c60b;hmacsha256"

#define MQTT_SERVER     "ON74ZK6IWN.iotcloud.tencentdevices.com"
#define MQTT_PORT       "1883"

#define SUB_TOPIC       "$thing/down/property/PM62I41E39/ESP8266"
#define PUB_TOPIC       "$thing/up/property/PM62I41E39/ESP8266"

#define MQTT_MAX_BUFF       512

#define MQTT_WAIT_TIME_MS   5000



bool mqtt_init();
bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive);
void mqtt_disconnect();
bool mqtt_subscribe(uint8_t* sub_topic, uint16_t req_qos, uint16_t msgid);
bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid);
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload);
bool mqtt_ping();

#endif //__MQTT_H__
