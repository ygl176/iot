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

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include "data_template_client_json.h"
#include "lite-utils.h"
#include "qcloud_iot_api_export.h"


void init_request_params(RequestParams *pParams, Method method, OnRequestCallback callback, void *userContext, uint8_t timeout_sec) {
	pParams->method 			=		method;
	pParams->user_context 		= 		userContext;
	pParams->timeout_sec 		= 		timeout_sec;
	pParams->request_callback 	= 		callback;
}

int check_snprintf_return(int32_t returnCode, size_t maxSizeOfWrite) {

    if (returnCode >= maxSizeOfWrite) {
        return QCLOUD_ERR_JSON_BUFFER_TRUNCATED;
    } else if (returnCode < 0) { // 写入出错
        return QCLOUD_ERR_JSON;
    }

    return QCLOUD_RET_SUCCESS;
}

void insert_str(char *pDestStr, char *pSourceStr, int pos) {
    int len = strlen(pDestStr);
    int nlen = strlen(pSourceStr);
    int i;
    //目的字符串向后移位
    for (i = len - 1; i >= pos; --i) {
        *(pDestStr + i + nlen) = *(pDestStr + i);
    }

    int n;
    //拷贝源字符串
    for (n = 0; n < nlen; n++)
        *(pDestStr + pos + n) = *pSourceStr++;
    *(pDestStr + len + nlen) = 0;
}


static int _direct_update_value(char *value, DeviceProperty *pProperty) {

    int rc = QCLOUD_RET_SUCCESS;

    if (pProperty->type == JBOOL) {
    	rc = LITE_get_boolean(pProperty->data, value);
    } else if (pProperty->type == JINT32) {
    	rc = LITE_get_int32(pProperty->data, value);
    } else if (pProperty->type == JINT16) {
    	rc = LITE_get_int16(pProperty->data, value);
    }
    else if (pProperty->type == JINT8) {
    	rc = LITE_get_int8(pProperty->data, value);
    }
    else if (pProperty->type == JUINT32) {
    	rc = LITE_get_uint32(pProperty->data, value);
    } else if (pProperty->type == JUINT16) {
    	rc = LITE_get_uint16(pProperty->data, value);
    }
    else if (pProperty->type == JUINT8) {
    	rc = LITE_get_uint8(pProperty->data, value);
    }
    else if (pProperty->type == JFLOAT) {
    	rc = LITE_get_float(pProperty->data, value);
    } else if (pProperty->type == JDOUBLE) {
    	rc = LITE_get_double(pProperty->data, value);
    }else if(pProperty->type == JSTRING){
		rc = LITE_get_string(pProperty->data, value, pProperty->data_buff_len);
	}else if(pProperty->type == JOBJECT){
		Log_d("Json type wait to be deal,%s",value);
	}else{
		Log_e("pProperty type unknow,%d",pProperty->type);
	}

    return rc;
}

/**
 * 为JSON文档增加client token字段
 *
 * @param pJsonDocument             JSON串
 * @param maxSizeOfJsonDocument     JSON串最大长度
 * @return                          添加字段的长度
 */
static int32_t _add_client_token(char *pJsonDocument, size_t maxSizeOfJsonDocument, uint32_t *tokenNumber) {

    int32_t rc_of_snprintf = snprintf(pJsonDocument, maxSizeOfJsonDocument, "%s-%lu", iot_device_info_get()->product_id, (*tokenNumber)++);

    return rc_of_snprintf;
}


int put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type) {

    int rc;
    int32_t rc_of_snprintf = 0;
    size_t remain_size = 0;

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

#ifdef QUOTES_TRANSFER_NEED
	rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\\\"%s\\\":", pKey);
#else
	rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\":", pKey);
#endif

    rc = check_snprintf_return(rc_of_snprintf, remain_size);
    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    if (pData == NULL) {
        rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "null"T_",");
    } else {
        if (type == JINT32) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi32
                                      ""T_",", *(int32_t *) (pData));
        } else if (type == JINT16) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi16
                                      ""T_",", *(int16_t *) (pData));
        } else if (type == JINT8) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      "d"//PRIi8
                                      ""T_",", *(int8_t *) (pData));
        } else if (type == JUINT32) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu32
                                      ""T_",", *(uint32_t *) (pData));
        } else if (type == JUINT16) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu16
                                      ""T_",", *(uint16_t *) (pData));
        } else if (type == JUINT8) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu8
                                      ""T_",", *(uint8_t *) (pData));
        } else if (type == JDOUBLE) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f"T_",", *(double *) (pData));
        } else if (type == JFLOAT) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f"T_",", *(float *) (pData));
        } else if (type == JBOOL) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%s"T_",",
                                      *(bool *) (pData) ? "true" : "false");
        } else if (type == JSTRING) {
#ifdef QUOTES_TRANSFER_NEED			
			rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\\\"%s\\\""T_",", (char *) (pData));
#else
			rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\",", (char *) (pData));
#endif
        } else if (type == JOBJECT) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%s"T_",", (char *) (pData));
        }
    }

    rc = check_snprintf_return(rc_of_snprintf, remain_size);

    return rc;
}


int template_put_json_node(char *jsonBuffer, size_t sizeOfBuffer, const char *pKey, void *pData, JsonDataType type) {

    int rc;
    int32_t rc_of_snprintf = 0;
    size_t remain_size = 0;

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

#ifdef QUOTES_TRANSFER_NEED
	rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\\\"%s\\\":", pKey);
#else
    rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\":", pKey);
#endif
    rc = check_snprintf_return(rc_of_snprintf, remain_size);
    if (rc != QCLOUD_RET_SUCCESS) {
        return rc;
    }

    if ((remain_size = sizeOfBuffer - strlen(jsonBuffer)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SMALL;
    }

    if (pData == NULL) {
        rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "null"T_",");
    } else {
        if (type == JINT32) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi32
                                      ""T_",", *(int32_t *) (pData));
        } else if (type == JINT16) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi16
                                      ""T_",", *(int16_t *) (pData));
        } else if (type == JINT8) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIi8
                                      ""T_",", *(int8_t *) (pData));
        } else if (type == JUINT32) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu32
                                      ""T_",", *(uint32_t *) (pData));
        } else if (type == JUINT16) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu16
                                      ""T_",", *(uint16_t *) (pData));
        } else if (type == JUINT8) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%"
                                      PRIu8
                                      ""T_",", *(uint8_t *) (pData));
        } else if (type == JDOUBLE) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f"T_",", *(double *) (pData));
        } else if (type == JFLOAT) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%f"T_",", *(float *) (pData));
		} else if (type == JBOOL) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%u"T_",",
                                      *(bool *) (pData) ? 1 : 0);		
        } else if (type == JSTRING) {
#ifdef QUOTES_TRANSFER_NEED			
			rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\\\"%s\\\""T_",", (char *) (pData));
#else
			rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "\"%s\",", (char *) (pData));
#endif
        } else if (type == JOBJECT) {
            rc_of_snprintf = snprintf(jsonBuffer + strlen(jsonBuffer), remain_size, "%s"T_",", (char *) (pData));
        }
    }

    rc = check_snprintf_return(rc_of_snprintf, remain_size);

    return rc;
}



int generate_client_token(char *pStrBuffer, size_t sizeOfBuffer, uint32_t *tokenNumber) {
    return _add_client_token(pStrBuffer, sizeOfBuffer, tokenNumber);
}

void build_empty_json(uint32_t *tokenNumber, char *pJsonBuffer) {
#ifdef QUOTES_TRANSFER_NEED
	snprintf(pJsonBuffer, MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN, "{\\\"clientToken\\\":\\\"%s-%u\\\"}", iot_device_info_get()->product_id, (*tokenNumber)++);
#else
	snprintf(pJsonBuffer, MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN, "{\"clientToken\":\"%s-%lu\"}", iot_device_info_get()->product_id, (*tokenNumber)++);
#endif	
}

bool parse_client_token(char *pJsonDoc, char **pClientToken) {
	*pClientToken = LITE_json_value_of(CLIENT_TOKEN_FIELD, pJsonDoc);
	return *pClientToken == NULL ? false : true;
}

bool parse_action_id(char *pJsonDoc, char **pActionID) {
	*pActionID = LITE_json_value_of(ACTION_ID_FIELD, pJsonDoc);
	return *pActionID == NULL ? false : true;
}

bool parse_timestamp(char *pJsonDoc, int32_t *pTimestamp) {

	bool ret = false;

	char *timestamp = LITE_json_value_of(TIME_STAMP_FIELD, pJsonDoc);
	if (timestamp == NULL) return false;

	if (sscanf(timestamp, "%" SCNu32, pTimestamp) != 1) {
		Log_e("parse code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
	}
	else {
		ret = true;
	}

	HAL_Free(timestamp);

	return ret;
}

bool parse_action_input(char *pJsonDoc, char **pActionInput)
{
    *pActionInput = LITE_json_value_of(CMD_CONTROL_PARA, pJsonDoc);
	return *pActionInput == NULL ? false : true;
}

bool parse_code_return(char *pJsonDoc, int32_t *pCode) {

	bool ret = false;

	char *code = LITE_json_value_of(REPLY_CODE, pJsonDoc);
	if (code == NULL) return false;
	if (sscanf(code, "%" SCNi32, pCode) != 1) {
		Log_e("parse code failed, errCode: %d", QCLOUD_ERR_JSON_PARSE);
	}
	else {
		ret = true;
	}

	HAL_Free(code);

	return ret;
}

bool parse_status_return(char *pJsonDoc, char **pStatus) {
	*pStatus = LITE_json_value_of(REPLY_STATUS, pJsonDoc);
	return *pStatus == NULL ? false : true;
}

bool update_value_if_key_match(char *pJsonDoc, DeviceProperty *pProperty) {

	bool ret = false;

	char* property_data = LITE_json_value_of(pProperty->key, pJsonDoc);
	if ((property_data == NULL) || !(strncmp(property_data, "null", 4))
		 ||!(strncmp(property_data, "NULL", 4))) {
	}
	else {		
		_direct_update_value(property_data, pProperty);
		ret = true;
		HAL_Free(property_data);
	}

	return ret;
}

bool parse_template_method_type(char *pJsonDoc, char **pMethod)
{
	*pMethod = LITE_json_value_of(METHOD_FIELD, pJsonDoc);
	return *pMethod == NULL ? false : true;
}

bool parse_template_get_control(char *pJsonDoc, char **control)
{
    *control = LITE_json_value_of(GET_CONTROL_PARA, pJsonDoc);
	return *control == NULL ? false : true;
}

bool parse_template_cmd_control(char *pJsonDoc, char **control)
{
    *control = LITE_json_value_of(CMD_CONTROL_PARA, pJsonDoc);
	return *control == NULL ? false : true;
}

#ifdef __cplusplus
}
#endif
