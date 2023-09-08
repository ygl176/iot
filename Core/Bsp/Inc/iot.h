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

typedef struct
{
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
 * @brief iot超时请求处理
 * 
 * @param client 
 */
void handle_timeout_repaly(iot_client_type *client);

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

#ifdef __cplusplus
}
#endif

#endif //__IOT_H__