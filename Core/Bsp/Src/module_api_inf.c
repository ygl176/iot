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
	if(!mqtt_connect(CLIENT_ID, DEVICE_NAME, DEVICE_KEY, 120))
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

	if(strlen(payload) > 1000){
		Log_e("payload too long");
	}else{
		if(!mqtt_publish(topic, payload))//ֻ֧�� Qos0, Qos1��ʵ��
		{
			Log_e("cmd mqtt_publish exec err");
			result = QCLOUD_ERR_FAILURE;
		}
	}
	
	return result;
}

/*******��������Ϣ��ʵ��**********/
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

	//ע�ᶩ�������
	result = register_sub_topic(&SubsParams);
	if(QCLOUD_RET_SUCCESS !=  result)
	{
		Log_e("register sub topic err,ret:%d",result);
		return result;
	}

	if(!mqtt_subscribe(topic, eQos, msg_id++))
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
	if(!mqtt_unsubscribe(topic, msg_id++))		
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
	return mqtt_ping();
}

// /*
// *ģ��������NB/2/3/4Gע�����磩��wifi������һ������/softAP����ʱ����ͳһ,��Ҫ�û����ݾ���ģ�����䡣
// * �����߲��� module_handshake APIʹ��AT��ܵ�API��ģ�齻����ʵ�����䡣
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

// 		/*�˴�ʾ�������ȵ�����ֱ��������ͨ�����������ض���Ʒ�����ض����¼���Ʃ�簴��������wifi������һ������/softAP��*/
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
	
// 		/*ģ������ע�ᡢ����wifi������Ҫ�û�������ѡģ��ʵ��*/			
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
	
// 		/*ģ������ע�ᡢ����wifi������Ҫ�û�������ѡģ��ʵ��*/			
// 		result = L206_net_reg();
// 		if(QCLOUD_RET_SUCCESS != result)
// 		{
// 			Log_e("L206 register network fail,ret:%d", result);	
// 		}	
// 	}
// #endif	


// 	return result;
// }




