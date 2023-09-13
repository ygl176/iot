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

#ifndef QCLOUD_IOT_EXPORT_ERROR_H_
#define QCLOUD_IOT_EXPORT_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * AT SDK ������
 */

typedef enum _eAtResault_{
    QCLOUD_RET_SUCCESS   									 = 0,      // ��ʾ�ɹ�����
    QCLOUD_ERR_FAILURE   									 = -100,   // ��ʾʧ�ܷ���
    QCLOUD_ERR_INVAL     									 = -101,   // ��ʾ������Ч����
    QCLOUD_ERR_NULL       									 = -102,   // ��ʾ��ָ��
    QCLOUD_ERR_MALLOC     									 = -103,   // ��ʾ�ڴ�����ʧ��
    QCLOUD_ERR_SEND_DATA   									 = -104,   // ��ʾAT���ڷ���ʧ��
   

	QCLOUD_ERR_TIMEOUT	  									 = -201,	//��Ӧ��ʱ	
	QCLOUD_ERR_RESP_NULL  									 = -202,	//��Ӧ����ָ����Ч
	QCLOUD_ERR_EXCEED_MAX_TOPICS 							 = -203,  //�������topic�ɶ�����
	QCLOUD_ERR_MQTT_NO_CONN									 = -204,  //MQTTδ����


    QCLOUD_ERR_JSON_PARSE                                    = -301,    // ��ʾJSON��������
    QCLOUD_ERR_JSON_BUFFER_TRUNCATED                         = -302,    // ��ʾJSON�ĵ��ᱻ�ض�
    QCLOUD_ERR_JSON_BUFFER_TOO_SMALL                         = -303,    // ��ʾ�洢JSON�ĵ��Ļ�����̫С
    QCLOUD_ERR_JSON                                          = -304,    // ��ʾJSON�ĵ����ɴ���
    QCLOUD_ERR_MAX_JSON_TOKEN                                = -305,    // ��ʾ����JSON�ĵ��е����Token��
    QCLOUD_ERR_MAX_APPENDING_REQUEST                         = -306,    // ��ʾ����ͬʱ�����ĵ�����
    QCLOUD_ERR_MAX_TOPIC_LENGTH                              = -307,    // ��ʾ�����涨����topic����


	QCLOUD_ERR_PROPERTY_EXIST                        		 = -401,    // ��ʾע��������Ѿ�����
    QCLOUD_ERR_PROPERTY_NOT_EXIST                     		 = -402,    // ��ʾע������Բ�����
    QCLOUD_ERR_REPORT_TIMEOUT               		         = -403,    // ��ʾ�����豸Ӱ���ĵ���ʱ
    QCLOUD_ERR_REPORT_REJECTED                     			 = -404,    // ��ʾ�����豸Ӱ���ĵ����ܾ�
    QCLOUD_ERR_GET_TIMEOUT                            	     = -405,    // ��ʾ��ȡ�豸Ӱ���ĵ���ʱ
    QCLOUD_ERR_GET_REJECTED                          	     = -406,    // ��ʾ��ȡ�豸Ӱ���ĵ����ܾ�
	QCLOUD_ERR_ACTION_EXIST                                  = -407,    // acion already exist
    QCLOUD_ERR_ACTION_NOT_EXIST                              = -408,    // acion not exist
} eAtResault;


#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_ERROR_H_ */
