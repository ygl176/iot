/**
 ******************************************************************************
 * @file			: module_api_inf.c
 * @brief			: api for application based on at
 ******************************************************************************
 *
 * History:      <author>          <time>        <version>
 *               yougaliu          2019-3-20        1.0
 * Desc:          ORG.
 * Copyright (c) 2019 Tencent. 
 * All rights reserved.
 ******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include "mqtt.h"
#include "module_api_inf.h"
#include "qcloud_iot_api_export.h"
#include "utils_timer.h"

static uint16_t msg_id;

/* mqtt setup connect */
eAtResault module_mqtt_conn(MQTTInitParams init_params)
{
	if(!mqtt_connect((uint8_t*)CLIENT_ID, (uint8_t*)DEVICE_NAME, (uint8_t*)DEVICE_KEY, 120))
	{
		Log_e("cmd mqtt_connect exec err");
		return QCLOUD_ERR_FAILURE;
	}
	return QCLOUD_RET_SUCCESS;
}


/* mqtt disconn */
eAtResault module_mqtt_discon(void)
{
	mqtt_disconnect();
}

/* mqtt pub msg */
eAtResault module_mqtt_pub(const char *topic, QoS eQos, char *payload)
{
	eAtResault result = QCLOUD_RET_SUCCESS;

	if(strlen(payload) > 200){
		Log_e("payload too long");
	}else{
		if(!mqtt_publish((uint8_t*)topic, (uint8_t*)payload))//只支持 Qos0, Qos1待实现
		{
			Log_e("cmd mqtt_publish exec err");
			result = QCLOUD_ERR_FAILURE;
		}
	}
	
	return result;
}

/*******发布长消息待实现**********/
// eAtResault module_mqtt_publ(const char *topic, QoS eQos, char *payload)
// {
// 	eAtResault result = QCLOUD_RET_SUCCESS;
// 	at_response_t resp = NULL;
// 	int len;
	
// 	resp = at_create_resp(64, 0, CMD_TIMEOUT_MS);
// 	at_set_end_sign('>');

// 	result = at_exec_cmd(resp, "AT+TCMQTTPUBL=\"%s\",%d,%d",topic, eQos, strlen(payload));
// 	if(QCLOUD_RET_SUCCESS !=  result)	
// 	{
// 		Log_e("cmd AT+TCMQTTPUBL exec err");
// 		goto exit;
// 	}
	
// 	len = at_client_send(at_client_get(), payload, strlen(payload));
// 	at_client_send(at_client_get(), "\r\n", 2);
// 	if(strlen(payload) !=  len)	
// 	{
// 		result = QCLOUD_ERR_SEND_DATA;
// 		Log_e("send data err");
// 	}
	

// exit:
// 	at_set_end_sign(0);
//     return QCLOUD_RET_SUCCESS;
// }


eAtResault module_mqtt_sub(char *topic, QoS eQos, OnMessageHandler cb, void *contex)
{
	eAtResault result = QCLOUD_RET_SUCCESS;
	SubscribeParams SubsParams;

	SubsParams.topicFilter = topic;
	SubsParams.eqos = eQos;
	SubsParams.fp = cb;
	SubsParams.context = contex;

	//注册订阅主题表
	result = register_sub_topic(&SubsParams);
	if(QCLOUD_RET_SUCCESS !=  result)
	{
		Log_e("register sub topic err,ret:%d",result);
		return result;
	}

	if(!mqtt_subscribe((uint8_t*)topic, eQos, msg_id++))
	{
		Log_e("cmd mqtt_subscribe exec err");
		result = QCLOUD_ERR_FAILURE;
	}
	else
	{
		Log_i("%s sub success", topic);
	}
	
	return result;
}

eAtResault module_mqtt_unsub(const char *topic)
{
	if(!mqtt_unsubscribe((uint8_t*)topic, msg_id++))
	{
		Log_e("send unsub msg err");
		return QCLOUD_ERR_FAILURE;
	}
	
	return QCLOUD_RET_SUCCESS;
}


eAtResault module_mqtt_state(eMqtt_State *pState)
{
	if(mqtt_ping())
		return QCLOUD_RET_SUCCESS;
	else
		return QCLOUD_ERR_FAILURE;
}


bool IOT_MQTT_IsConnected(void) 
{
	HAL_Delay(1000);
	return mqtt_ping();
}

// /*
// *模组联网（NB/2/3/4G注册网络）、wifi配网（一键配网/softAP）暂时很难统一,需要用户根据具体模组适配。
// * 开发者参照 module_handshake API使用AT框架的API和模组交互，实现适配。
// */	
// eAtResault module_register_network(eModuleType eType)
// {
// 	eAtResault result = QCLOUD_RET_SUCCESS;
	
// #ifdef MODULE_TYPE_ESP8266	
// 	if(eType == eMODULE_ESP8266)
// 	{
// 		#define WIFI_SSID	"L-0426"
// 		#define WIFI_PW		"qazwsx11"

// 		result = wifi_set_cwmod(1);
// 		if(QCLOUD_RET_SUCCESS != result)
// 		{
// 			Log_e("set cwmode err,ret:%d", result);	
// 		}

// 		/*此处示例传递热点名字直接联网，通常的做法是特定产品根据特定的事件（譬如按键）触发wifi配网（一键配网/softAP）*/
// 		result = wifi_connect(WIFI_SSID, WIFI_PW);
// 		if(QCLOUD_RET_SUCCESS != result)
// 		{
// 			Log_e("wifi connect fail,ret:%d", result);	
// 		}
// 	}
// #endif

// #ifdef 	MODULE_TYPE_N21
// 	if(eType == eMODULE_N21)
// 	{
	
// 		/*模组网络注册、或者wifi配网需要用户根据所选模组实现*/			
// 		result = N21_net_reg();
// 		if(QCLOUD_RET_SUCCESS != result)
// 		{
// 			Log_e("N21 register network fail,ret:%d", result);	
// 		}	
// 	}
// #endif	

// #ifdef 	MODULE_TYPE_L206D
// 		if(eType == eMODULE_L206D)
// 		{
	
// 		/*模组网络注册、或者wifi配网需要用户根据所选模组实现*/			
// 		result = L206_net_reg();
// 		if(QCLOUD_RET_SUCCESS != result)
// 		{
// 			Log_e("L206 register network fail,ret:%d", result);	
// 		}	
// 	}
// #endif	


// 	return result;
// }




