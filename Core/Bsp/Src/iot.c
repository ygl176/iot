/**
 * @file ioc.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "iot.h"
#include "log.h"
#include "mqtt.h"

static iot_client_type iot_client;

iot_client_type *iot_get_client()
{
    return &iot_client;
}


bool iot_client_init(iot_client_type *client)
{
    if(client == NULL)
    {
        Log_e("client is NULL");
        return false;
    }

    client->lock = HAL_MutexCreate();
    if(client->lock == NULL)
    {
        Log_e("iot client mutex creat false");
        return false;
    }

    client->pMsgList = list_new();

    if(client->pMsgList)
    {
        client->pMsgList->free = HAL_Free;
    }
    else
    {
        Log_e("iot pMsgList malloc failed");
        return false;
    }

    client->property_handle_list = list_new();

    if(client->property_handle_list)
    {
        client->property_handle_list->free = HAL_Free;
    }
    else
    {
        Log_e("iot property List malloc failed");
        return false;
    }

    client->reply_list = list_new();

    if(client->reply_list)
    {
        client->reply_list->free = HAL_Free;
    }
    else
    {
        Log_e("iot replay list malloc failed");
        return false;
    }

    client->sub_topic = list_new();

    if(client->sub_topic)
    {
        client->sub_topic->free = HAL_Free;
    }
    else
    {
        Log_e("iot replay list malloc failed");
        return false;
    }

    return true;
}


void iot_client_reset(iot_client_type *client)
{
    if(client->property_handle_list)
    {
        list_destroy(client->property_handle_list);
    }
}

void handle_timeout_repaly(iot_client_type *client)
{

}


bool iot_send_request(iot_client_type *client)
{
    
}


bool iot_sub_down_topic(iot_client_type* client)
{

}

