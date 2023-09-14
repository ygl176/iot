/**
 ******************************************************************************
 * @file			: mqtt.c
 * @brief			: mqtt logic deal
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
#include "MQTTPacket.h"
#include "qcloud_iot_api_export.h"

static SubscribeParams sg_msg_handlers[QCLOUD_IOT_MAX_SUB_TOPIC] = {0}; /**< 订阅消息回调*/
static bool sg_mqtt_lock = false;  //To do:  use mutex instand


eAtResault register_sub_topic(SubscribeParams* subpara) 
{
	POINTER_SANITY_CHECK(subpara, QCLOUD_ERR_NULL); 
	
	int i;
    eAtResault rc = QCLOUD_ERR_FAILURE;

	if(sg_mqtt_lock)
	{
		Log_e("premsg is dealing, sub topic to be wait");
		return QCLOUD_ERR_FAILURE; 
	}
	
	sg_mqtt_lock = true;
		
    for (i = 0; i < QCLOUD_IOT_MAX_SUB_TOPIC; ++i) 
	{
		if((NULL != sg_msg_handlers[i].topicFilter)&&(0 == strcmp(sg_msg_handlers[i].topicFilter,subpara->topicFilter)))
		{
            if (subpara->fp == NULL) 
			{
				HAL_Free(sg_msg_handlers[i].topicFilter);
                sg_msg_handlers[i].topicFilter = NULL;
                sg_msg_handlers[i].fp = NULL;
            }
            rc = QCLOUD_RET_SUCCESS;
            break;
        }
    }
	
    if (subpara->fp != NULL) 
	{
        if (rc == QCLOUD_ERR_FAILURE) 
		{
            for (i = 0; i < QCLOUD_IOT_MAX_SUB_TOPIC; ++i) 
			{
                if (sg_msg_handlers[i].topicFilter == NULL) 
				{
                    rc = QCLOUD_RET_SUCCESS;
                    break;
                }
            }
        }
		
        if (i < QCLOUD_IOT_MAX_SUB_TOPIC) 
		{
			char *pTopic = HAL_Malloc(strlen(subpara->topicFilter));
			if(pTopic)
			{
				memset(pTopic, 0, strlen(subpara->topicFilter));
				strcpy(pTopic, subpara->topicFilter);
	            sg_msg_handlers[i].topicFilter = pTopic;
	            sg_msg_handlers[i].fp = subpara->fp;
	            sg_msg_handlers[i].eqos = subpara->eqos;
	            sg_msg_handlers[i].context = subpara->context;
			}
			else
			{
				Log_e("malloc err");
				rc = QCLOUD_ERR_MALLOC;
			}
			 

        }
		else
		{
			 rc = QCLOUD_ERR_EXCEED_MAX_TOPICS;
		}
    }
	else
	{
		 rc = QCLOUD_RET_SUCCESS;
	}

	sg_mqtt_lock = false;
	
    return rc;
}

/**
 * @brief 消息主题是否相同
 *
 * @param topic_filter
 * @param topicName
 * @return
 */
static uint8_t _is_topic_equals(char *topic_filter, char *topicName) {
    return (uint8_t) (strlen(topic_filter) == strlen(topicName) && !strcmp(topic_filter, topicName));
}


/**
 * @brief 消息主题匹配
 *
 * assume topic filter and name is in correct format
 * # can only be at end
 * + and # can only be next to separator
 *
 * @param topic_filter   订阅消息的主题名
 * @param topicName     收到消息的主题名, 不能包含通配符
 * @param topicNameLen  主题名的长度
 * @return
 */
static uint8_t _is_topic_matched(char *topic_filter, char *topicName, uint16_t topicNameLen) {
    char *curf;
    char *curn;
    char *curn_end;

    curf = topic_filter;
    curn = topicName;
    curn_end = curn + topicNameLen;

    while (*curf && (curn < curn_end)) {

        if (*curf == '+' && *curn == '/') {
            curf++;
            continue;
        }

        if (*curn == '/' && *curf != '/') {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) {
            break;
        }

        if (*curf == '+') {
            /* skip until we meet the next separator, or end of string */
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        } else if (*curf == '#') {
            /* skip until end of string */
            curn = curn_end - 1;
        }

        curf++;
        curn++;
    };

    if (*curf == '\0') {
        return (uint8_t) (curn == curn_end);
    } else {
        return (uint8_t) ((*curf == '#') || *(curf + 1) == '#' || (*curf == '+' && *(curn - 1) == '/'));
    }
}


/**
 * @brief 终端收到服务器的的PUBLISH消息之后, 传递消息给消息回调处理函数
 *
 * @param data 收到的消息 
 * eg：
 *+TCMQTTRCVPUB:"03UKNYBUZG/dev2/data",39,"{"action":"publish_test","count":"954"}"
 * 
 * @param size 收到的消息长度
 * @return @see eAtResault
 */
eAtResault deliver_message(const char *data, uint32_t size)
{
    POINTER_SANITY_CHECK(data, QCLOUD_ERR_NULL);
	
    uint8_t dup;
    int qos;
    uint8_t retained;
    uint16_t packetid;
    MQTTString topicName;
    uint8_t *payload;
	int	len = 0;
	int i = 0;
	bool matchFlag = false;
    char topic_name[MAX_TOPIC_NAME_LEN+1] = {0};
    char topic_payload[MAX_TOPIC_PAYLOAD_LEN+1] = {0};
	eAtResault Ret;

	if(sg_mqtt_lock)
	{
		Log_e("premsg is dealing");
		return QCLOUD_ERR_FAILURE; 
	}
	sg_mqtt_lock = true;
	
	/* parameters parsing */
    if(MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &len, (uint8_t*)data, size) != 1)
    {
        Log_e("input msg format illegal");
        Ret = QCLOUD_ERR_FAILURE;
    }
    else
	{
		//Log_d("msg_topic:%s, len:%u, payload:%s", topic_name, len, topic_payload);
	}

    if(topicName.lenstring.len < MAX_TOPIC_NAME_LEN)
    {
        memcpy(topic_name, topicName.lenstring.data, topicName.lenstring.len);
    }
    else
    {
        Log_e("topic name too long");
        Ret = QCLOUD_ERR_FAILURE;
    }

    if(len < MAX_TOPIC_PAYLOAD_LEN)
    {
        memcpy(topic_payload, payload, len);
    }
    else
    {
        Log_e("topic payload too long");
        Ret = QCLOUD_ERR_FAILURE;
    }
	
	for (i = 0; i < QCLOUD_IOT_MAX_SUB_TOPIC; i++) 
	{
        if ((sg_msg_handlers[i].topicFilter != NULL)
            && (_is_topic_equals(topic_name, sg_msg_handlers[i].topicFilter) ||
                _is_topic_matched((char *) sg_msg_handlers[i].topicFilter, topic_name, strlen(topic_name))))
        {      	
            if (sg_msg_handlers[i].fp != NULL) 
			{
				matchFlag = true;
                sg_msg_handlers[i].fp(topic_payload,  sg_msg_handlers[i].context);
				break;
            }
        }
    }

	if(!matchFlag)
	{
		Log_d("no matching any topic, call default handle function");
		//to do
	}

	Ret = QCLOUD_RET_SUCCESS;     
	sg_mqtt_lock = false;
	
	return Ret;
}


