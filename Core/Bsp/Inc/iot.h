/**
 * @file iot.h
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __IOT_H__
#define __IOT_H__

#include "common.h"
#include "utils_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    IOT_UNINITIALIZED = 0,  //未初始化
    IOT_INITIALIZED,        //初始化
    IOT_BUSY,               //忙
}iot_status;

typedef struct
{
    iot_status status;
    void *lock;
    List *pMsgList;
    List *reply_list;
    List *property_handle_list;
    List *sub_topic;
} iot_client_type;


iot_client_type *iot_get_client();


/**
 * @brief iot抽象初始化
 * 
 * @param client 
 * @return true 
 * @return false 
 */
bool iot_client_init(iot_client_type *client);

/**
 * @brief iot抽象释放
 * 
 * @param client 
 */
void iot_client_reset(iot_client_type *client);


/**
 * @brief iot 用户端发送请求信息
 * 
 * @param client 
 * @return true 
 * @return false 
 */
bool iot_send_request(iot_client_type *client);

/**
 * @brief iot 用户端订阅下行主题
 * 
 * @param client 
 * @return true 
 * @return false 
 */
bool iot_sub_down_topic(iot_client_type* client);


bool iot_register_property();


bool iot_unregister_property();


/***********************消息处理***********************/

/**
 * @brief iot超时请求处理
 * 
 * @param client 
 */
void iot_handle_timeout_repaly(iot_client_type *client);


void iot_handle_message(const char* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif //__IOT_H__