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

#ifndef QCLOUD_IOT_EXPORT_METHOD_H_
#define QCLOUD_IOT_EXPORT_METHOD_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ������Ӧ���ص�����
 */
typedef enum {
    ACK_NONE = -3,      // ����ʱ
    ACK_TIMEOUT = -2,   // ����ʱ
    ACK_REJECTED = -1,  // ����ܾ�
    ACK_ACCEPTED = 0    // �������
} RequestAck;

typedef RequestAck ReplyAck;

/**
 * @brief �����ƶ��豸�ĵ�����ʹ�õ����ַ�ʽ
 */
typedef enum {
    GET = 0,     // ��ȡ�ƶ��豸�ĵ�
    REPORT = 1,  // ���»򴴽��ƶ��豸�ĵ�    
    RINFO = 2,   // �ϱ�ϵͳ��Ϣ
    REPLY = 3,   // ����ģ������������Ϣ�ظ�
    CLEAR = 4,   // ɾ�������ڼ��control����
} Method;

/**
 * @brief JSON�ĵ���֧�ֵ���������
 */
typedef enum {
    JINT32,     // 32λ�з�������
    JINT16,     // 16λ�з�������
    JINT8,      // 8λ�з�������
    JUINT32,    // 32λ�޷�������
    JUINT16,    // 16λ�޷�������
    JUINT8,     // 8λ�޷�������
    JFLOAT,     // �����ȸ�����
    JDOUBLE,    // ˫���ȸ�����
    JBOOL,      // ������
    JSTRING,    // �ַ���
    JOBJECT     // JSON����
} JsonDataType;

/**
 * @brief �����豸��ĳ������, ʵ�ʾ���һ��JSON�ĵ��ڵ�
 */
typedef struct _JSONNode {
    char   		 *key;    // ��JSON�ڵ��Key
    void         *data;   // ��JSON�ڵ��Value
    uint16_t     data_buff_len;  // data buff len, for string type value update
    JsonDataType type;    // ��JSON�ڵ����������
} DeviceProperty;


/**
 * @brief Define a device action
 */
typedef struct {
    char    *pActionId;     // action id
    uint32_t timestamp;	    // action timestamp
    uint8_t  input_num;     // input num
    uint8_t  output_num;    // output mun
    DeviceProperty *pInput;  // input 
    DeviceProperty *pOutput; // output
} DeviceAction;


/**
 * @brief ÿ���ĵ�������Ӧ�Ļص�����
 *
 * @param method         �ĵ�������ʽ
 * @param requestAck     ������Ӧ����
 * @param pJsonDocument  �ƶ���Ӧ���ص��ĵ�
 * @param userContext      �û�����
 *
 */
typedef void (*OnRequestCallback)(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument, void *userContext);


/**
 * @brief  ������Ϣ��Ӧ�Ļص�����
 *
 * @param method         �ظ�����Ϣ��������
 * @param replyAck       ������Ӧ����
 * @param pJsonDocument  �ƶ���Ӧ���ص��ĵ�
 * @param userContext      �û�����
 *
 */
typedef void (*OnReplyCallback)(void *pClient, Method method, ReplyAck replyAck, const char *pJsonDocument, void *userContext);


/**
 * @brief �豸���Դ���ص�����
 *
 * @param pJsonValueBuffer �豸����ֵ
 * @param valueLength      �豸����ֵ����
 * @param DeviceProperty   �豸���Խṹ��
 */
typedef void (*OnPropRegCallback)(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty);


/**
 * @brief action handle callback 
 *
 * @param pAction   action with input data
 */
typedef void (*OnActionHandleCallback)(void *pClient, const char *pClientToken, DeviceAction *pAction);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_METHOD_H_ */
