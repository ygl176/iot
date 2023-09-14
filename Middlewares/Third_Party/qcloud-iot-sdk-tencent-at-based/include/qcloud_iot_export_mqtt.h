/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
 
#ifndef QCLOUD_IOT_EXPORT_MQTT_H_
#define QCLOUD_IOT_EXPORT_MQTT_H_
#include "stdint.h"
#include "qcloud_iot_export_error.h"



#define 	QCLOUD_IOT_MAX_SUB_TOPIC 								(5)		//���ɶ���topic��
#define 	MAX_TOPIC_NAME_LEN     									(138)	//topic�������
#define 	MAX_TOPIC_PAYLOAD_LEN   								(200)  //topic �payload

/* �豸ID����󳤶�, ���뱣��Ψһ */
#define 	MAX_SIZE_OF_CLIENT_ID                                   (80)

/* MQTT ��������(��������, ����, ������)�ĳ�ʱʱ��, ��λ:ms ����5000ms */
#define QCLOUD_IOT_MQTT_COMMAND_TIMEOUT                             (10 * 1000)


/* ��Ʒ���Ƶ���󳤶� */
#define MAX_SIZE_OF_PRODUCT_ID                                    	(10)

/* ��Ʒ��Կ����󳤶� ����̬�豸ע����Ҫ*/
#define MAX_SIZE_OF_PRODUCT_SECRET                                  (48)


/* �豸ID����󳤶� */
#define MAX_SIZE_OF_DEVICE_NAME                                     (64)

/* psk��󳤶� */
#define MAX_SIZE_OF_DEVICE_SERC  	 								(24)


/* �豸֤���ļ�������󳤶� */
#define MAX_SIZE_OF_DEVICE_CERT_FILE_NAME                           (128)

/* �豸˽Կ�ļ�������󳤶� */
#define MAX_SIZE_OF_DEVICE_KEY_FILE_NAME                            (128)


/* �ƶ˱����������󳤶� */
#define MAX_SIZE_OF_CLOUD_TOPIC            ((MAX_SIZE_OF_DEVICE_NAME) + (MAX_SIZE_OF_PRODUCT_ID) + 64)

/**
 * @brief �����Ȩ��ʽ
 */

typedef enum{
	eNO_TLS = 0,		//ֱ��,��TLS
	ePSK_TLS = 1,		//PSK��ʽTLS
	eCERT_TLS = 2,		//֤�鷽ʽTLS
}eTlsMode;

typedef enum _eClientState_{
	eCLIENT_UNINITIALIZED = 0,
	eCLIENT_INITIALIZED = 1,
}eClientState;


/**
 * @brief ���������ȼ�
 *
 * ���������ȼ���ʾPUBLISH��Ϣ�ַ��������ȼ�
 */
typedef enum _QoS {
    QOS0 = 0,    // ����ַ�һ��
    QOS1 = 1,    // ���ٷַ�һ��, ��Ϣ�Ľ�������ظ�PUBACK����
    QOS2 = 2     // ���ַ�һ��, Ŀǰ��Ѷ�����Ʋ�֧�ָõȼ�
} QoS;

typedef struct {
	eTlsMode 					tlsmod;
    uint32_t					command_timeout;		 // �������������д��ʱʱ�� ms
    uint32_t					keep_alive_interval_ms;	 // ��������, ��λ: s
    uint8_t         			clean_session;			 // ����Ự��־λ
    uint8_t                   	auto_connect_enable;     // �Ƿ����Զ����� 1:�����Զ����� 0���������Զ�����  ����Ϊ1
} MQTTInitParams;


/**
 * @brief MQTT��ʼ�������ṹ��Ĭ��ֵ����
 */
#define DEFAULT_MQTTINIT_PARAMS {ePSK_TLS, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT, 240, 1, 1}

/**
 * @brief MQTT PUBLISH ��Ϣ�ص�������ָ�붨��
 */
typedef void (*OnMessageHandler)(char *msg, void *context);


/**
 * @brief �ͻ�������� Topic ���������ҵ�񳡾���
 * ʵ�ʶ��ĵ���� Topic ������������á�
 */
typedef struct _SubscribeParams_ {
	char* topicFilter;
	QoS eqos;
	OnMessageHandler fp;
	void * context;
} SubscribeParams;


eAtResault register_sub_topic(SubscribeParams* subpara);
eAtResault deliver_message(const char *data, uint32_t size);


#endif
