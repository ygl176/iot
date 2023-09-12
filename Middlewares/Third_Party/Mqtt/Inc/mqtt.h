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

#define 	QCLOUD_IOT_MAX_SUB_TOPIC 								(5)		//最多可订阅topic数
#define 	MAX_TOPIC_NAME_LEN     									(138)	//topic名最长长度
#define 	MAX_TOPIC_PAYLOAD_LEN   								(1024)  //topic 最长payload

/* 设备ID的最大长度, 必须保持唯一 */
#define 	MAX_SIZE_OF_CLIENT_ID                                   (80)

/* 产品名称的最大长度 */
#define MAX_SIZE_OF_PRODUCT_ID                                    	(10)

/* 产品密钥的最大长度 ，动态设备注册需要*/
#define MAX_SIZE_OF_PRODUCT_SECRET                                  (48)


/* 设备ID的最大长度 */
#define MAX_SIZE_OF_DEVICE_NAME                                     (64)

/* psk最大长度 */
#define MAX_SIZE_OF_DEVICE_SERC  	 								(24)


/* 设备证书文件名的最大长度 */
#define MAX_SIZE_OF_DEVICE_CERT_FILE_NAME                           (128)

/* 设备私钥文件名的最大长度 */
#define MAX_SIZE_OF_DEVICE_KEY_FILE_NAME                            (128)

/* 云端保留主题的最大长度 */
#define MAX_SIZE_OF_CLOUD_TOPIC            ((MAX_SIZE_OF_DEVICE_NAME) + (MAX_SIZE_OF_PRODUCT_ID) + 64)

#define CLIENT_ID       (char*)"VOIXMXT1OKesp"
#define DEVICE_NAME     (char*)"VOIXMXT1OKesp;12010126;9dab2;1702137600"
#define DEVICE_KEY      (char*)"42866c6458d133519e19bc5cab65441bd5e3303d5c1638a7ffb559acf522cd15;hmacsha256"

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
typedef enum _QoS {
    QOS0 = 0,    // 至多分发一次
    QOS1 = 1,    // 至少分发一次, 消息的接收者需回复PUBACK报文
    QOS2 = 2     // 仅分发一次, 目前腾讯物联云不支持该等级
} QoS;

/**
 * @brief MQTT PUBLISH 消息回调处理函数指针定义
 */
typedef void (*OnMessageHandler)(char *msg, void *context);

//订阅主题控制块
typedef struct _SubscribeParams_ {
	char* topicFilter;
	QoS eqos;
	OnMessageHandler fp;
	void * context;
} SubscribeParams;



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

eResault register_sub_topic(SubscribeParams* subpara);
eResault deliver_message(const char *data, uint32_t size);

#endif //__MQTT_H__
