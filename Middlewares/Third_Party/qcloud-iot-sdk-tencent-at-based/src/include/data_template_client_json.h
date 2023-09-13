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

#ifndef UTILS_METHOD_H_
#define UTILS_METHOD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "utils_timer.h"
#include "qcloud_iot_export_method.h"
#include "qcloud_iot_export_mqtt.h"




#define min(a,b) (a) < (b) ? (a) : (b)


/* ���������ʱ����, ����appending״̬������������ */
#define MAX_APPENDING_REQUEST_AT_ANY_GIVEN_TIME                     (10)

/* ���������ʱ����, ����ͬʱ�����豸�������� */
#define MAX_DEVICE_HANDLED_AT_ANY_GIVEN_TIME                        (10)

/* ���豸����֮��, �ƶ˱����������󳤶� */
#define MAX_SIZE_OF_CLOUD_TOPIC_WITHOUT_DEVICE_NAME                 (60)

/* �����ƶ˷��ص�JSON�ĵ���buffer��С */
#define CLOUD_IOT_JSON_RX_BUF_LEN                                   (MAX_TOPIC_PAYLOAD_LEN + 1)


/* һ��clientToken����󳤶� */
#define MAX_SIZE_OF_CLIENT_TOKEN                                    (MAX_SIZE_OF_CLIENT_ID + 10)

/* һ��������clientToken�ֶε�JSON�ĵ�����󳤶� */
#define MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN                          (MAX_SIZE_OF_CLIENT_TOKEN + 20)


#define CLIENT_TOKEN_FIELD     		"clientToken"
#define METHOD_FIELD	         	"method"
#define TYPE_FIELD	         		"type"
#define ACTION_ID_FIELD     		"actionId"
#define TIME_STAMP_FIELD     		"timestamp"

#define REPLY_CODE					"code"
#define REPLY_STATUS				"status"

#define GET_STATUS					"get_status"			//�豸������ѯ����״̬
#define GET_STATUS_REPLY			"get_status_reply"		//����˻ظ�����״̬��ѯ

#define CONTROL_CMD					"control"				//��������п�������
#define CONTROL_CMD_REPLY			"control_reply"			//�豸�ظ���������

#define CLEAR_CONTROL				"clear_control"

#define REPORT_CMD					"report"				//�豸�����ϱ�����
#define REPORT_CMD_REPLY			"report_reply"			//����˻ظ������ϱ�

#define INFO_CMD					"report_info"			//�豸��Ϣ�ϱ�
#define INFO_CMD_REPLY				"report_info_reply"		//�豸��Ϣ�ϱ��ظ�


#define GET_CONTROL_PARA			"data.control"
#define CMD_CONTROL_PARA			"params"

/**
 * @brief �ĵ���������Ĳ����ṹ�嶨��
 */
typedef struct _RequestParam {

    Method               	method;              	// �ĵ�����ʽ: GET, REPORT, RINFO, REPLY,CLEAR

    uint32_t             	timeout_sec;         	// ����ʱʱ��, ��λ:s

    OnRequestCallback    	request_callback;    	// ����ص�����

    void                 	*user_context;          // �û�����, ��ͨ���ص�����OnRequestCallback����

} RequestParams;

#define DEFAULT_REQUEST_PARAMS {GET, 4, NULL, NULL};

/**
 * @brief ����һ���ĵ�����
 */
typedef struct {
    char                   client_token[MAX_SIZE_OF_CLIENT_TOKEN];          // ��ʶ�������clientToken�ֶ�
    Method                 method;                                          // �ĵ�������ʽ

    void                   *user_context;                                   // �û�����
    Timer                  timer;                                           // ����ʱ��ʱ��

    OnRequestCallback      callback;                                        // �ĵ��������󷵻ش�����
} Request;

/**
 * @brief �������ɲ�ͬ������
 */
typedef enum {
    ACCEPTED, REJECTED, METHOD
} RequestType;


/**
 * @brief �ýṹ�����ڱ����ѵǼǵ��豸���Լ��豸���Դ���Ļص�����
 */
typedef struct {

    void *property;							// �豸����

    OnPropRegCallback callback;      // �ص�������

} PropertyHandler;


/**
 * @brief save the action registed and its callback
 */
typedef struct {

    void *action;							

    OnActionHandleCallback callback;      

} ActionHandler;


/**
 * @brief ��麯��snprintf�ķ���ֵ
 *
 * @param returnCode       ����snprintf�ķ���ֵ
 * @param maxSizeOfWrite   ��д����ֽ���
 * @return                 ����QCLOUD_ERR_JSON, ��ʾ����; ����QCLOUD_ERR_JSON_BUFFER_TRUNCATED, ��ʾ�ض�
 */

int check_snprintf_return(int32_t returnCode, size_t maxSizeOfWrite);

/**
 * @brief ��ʼ���������
 *
 * @param pParams   ����ʼ���������ָ��
 * @param method    ���󷽷�
 * @param callback  ����ص�
 * @param userContext   �û�����
 */
void init_request_params(RequestParams *pParams, Method method, OnRequestCallback callback, void *userContext, uint8_t timeout_sec);


/**
 * @brief ����Դ�������뵽Ŀ���ַ���ָ��λ��
 *
 * @param pSourceStr   Դ�ַ���
 * @param pDestStr     Ŀ������ַ���
 * @param pos          Ŀ�괮����λ��
 */
void insert_str(char *pDestStr, char *pSourceStr, int pos);


/**
 * ��һ��JSON�ڵ�д�뵽JSON����
 *
 * @param jsonBuffer   	JSON��
 * @param sizeOfBuffer  ��д���С
 * @param pKey          JSON�ڵ��key
 * @param pData         JSON�ڵ��value
 * @param type          JSON�ڵ�value����������
 * @return              ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type);

/**
 * ��һ��JSON�ڵ�д�뵽JSON����,��ģ�Ͷ�bool���͵Ĵ��������֡�
 *
 * @param jsonBuffer   	JSON��
 * @param sizeOfBuffer  ��д���С
 * @param pKey          JSON�ڵ��key
 * @param pData         JSON�ڵ��value
 * @param type          JSON�ڵ�value����������
 * @return              ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int template_put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type);


/**
 * @brief ����һ��ClientToken
 *
 * @param pStrBuffer    �洢ClientToken���ַ���������
 * @param sizeOfBuffer  ��������С
 * @param tokenNumber   shadow��tokenֵ�������ڲ�ÿ��ִ���������
 * @return              ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int generate_client_token(char *pStrBuffer, size_t sizeOfBuffer, uint32_t *tokenNumber);

/**
 * @brief ΪGET��DELETE������һ��ֻ����clientToken�ֶε�JSON�ĵ�
 *
 * @param tokenNumber   shadow��tokenֵ�������ڲ�ÿ��ִ���������
 * @param pJsonBuffer �洢JSON�ĵ����ַ���������
 */
void build_empty_json(uint32_t *tokenNumber, char *pJsonBuffer);

/**
 * @brief ��JSON�ĵ��н�����clientToken�ֶ�
 *
 * @param pJsonDoc       ��������JSON�ĵ�
 * @param pClientToken   ClientToken�ֶ�
 * @return               ����true, ��ʾ�����ɹ�
 */
bool parse_client_token(char *pJsonDoc, char **pClientToken);


/**
 * @brief parse field of aciont_id from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pActionID   	 pointer to field of action_id
 * @return               true for success
 */
bool parse_action_id(char *pJsonDoc, char **pActionID);

/**
 * @brief parse field of timestamp from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pTimestamp     pointer to field of timestamp
 * @return               true for success
 */
bool parse_timestamp(char *pJsonDoc, int32_t *pTimestamp);


/**
 * @brief parse field of input from JSON string
 *
 * @param pJsonDoc       source JSON string
 * @param pActionInput   filed of params as action input parameters
 * @return               true for success
 */ 
bool parse_action_input(char *pJsonDoc, char **pActionInput);


/**
 * @brief ��JSON�ĵ��н�����status�ֶ�,�¼��ظ�
 *
 * @param pJsonDoc       ��������JSON�ĵ�
 * @param pStatus   	 status�ֶ�
 * @return               ����true, ��ʾ�����ɹ�
 */
bool parse_status_return(char *pJsonDoc, char **pStatus);

/**
 * @brief ��JSON�ĵ��н�����code�ֶ�,�¼��ظ�
 *
 * @param pJsonDoc       ��������JSON�ĵ�
 * @param pCode   		 Code�ֶ�
 * @return               ����true, ��ʾ�����ɹ�
 */
bool parse_code_return(char *pJsonDoc, int32_t *pCode);


/**
 * @brief ���JSON�ĵ��е�key��ĳ���豸���Ե�keyƥ��Ļ�, ����¸��豸����, ���豸���Ե�ֵ����ΪOBJECT����
 *
 * @param pJsonDoc       JSON�ĵ�
 * @param pProperty      �豸����
 * @return               ����true, ��ʾ�ɹ�
 */
bool update_value_if_key_match(char *pJsonDoc, DeviceProperty *pProperty);

/**
 * @brief ��JSON�ĵ��н�����method�ֶ�
 *
 * @param pJsonDoc         ��������JSON�ĵ�
 * @param pErrorMessage    ���method�ֶ�
 * @return                 ����true, ��ʾ�����ɹ�
 */
bool parse_template_method_type(char *pJsonDoc, char **pMethod);

/**
 * @brief ��get_status_reply ��JSON�ĵ��н�����control�ֶ�
 *
 * @param pJsonDoc         ��������JSON�ĵ�
 * @param pErrorMessage    ���control�ֶ�
 * @return                 ����true, ��ʾ�����ɹ�
 */
bool parse_template_get_control(char *pJsonDoc, char **control);

/**
 * @brief ��control ��JSON�ĵ��н�����control�ֶ�
 *
 * @param pJsonDoc         ��������JSON�ĵ�
 * @param pErrorMessage    ���control�ֶ�
 * @return                 ����true, ��ʾ�����ɹ�
 */
bool parse_template_cmd_control(char *pJsonDoc, char **control);


#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_SHADOW_H_ */
