/**
 * @file export_err.h
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _EXPORT_ERROR_H_
#define _EXPORT_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef enum _eResault_{
    RET_SUCCESS   						        = 0,      // 表示成功返回
    ERR_FAILURE   						        = -100,   // 表示失败返回
    ERR_INVAL     						        = -101,   // 表示参数无效错误
    ERR_NULL       						        = -102,   // 表示空指针
    ERR_MALLOC     						        = -103,   // 表示内存申请失败
    ERR_SEND_DATA   				            = -104,   // 表示AT串口发送失败
   

	ERR_TIMEOUT	  						        = -201,	   //响应超时	
	ERR_RESP_NULL  						        = -202,	   //响应数据指针无效
	ERR_EXCEED_MAX_TOPICS 				        = -203,    //超过最多topic可订阅数
	ERR_MQTT_NO_CONN				            = -204,    //MQTT未连接


    ERR_JSON_PARSE                              = -301,    // 表示JSON解析错误
    ERR_JSON_BUFFER_TRUNCATED                   = -302,    // 表示JSON文档会被截断
    ERR_JSON_BUFFER_TOO_SMALL                   = -303,    // 表示存储JSON文档的缓冲区太小
    ERR_JSON                                    = -304,    // 表示JSON文档生成错误
    ERR_MAX_JSON_TOKEN                          = -305,    // 表示超过JSON文档中的最大Token数
    ERR_MAX_APPENDING_REQUEST                   = -306,    // 表示超过同时最大的文档请求
    ERR_MAX_TOPIC_LENGTH                        = -307,    // 表示超过规定最大的topic长度


	ERR_PROPERTY_EXIST                          = -401,    // 表示注册的属性已经存在
    ERR_PROPERTY_NOT_EXIST                      = -402,    // 表示注册的属性不存在
    ERR_REPORT_TIMEOUT                          = -403,    // 表示更新设备影子文档超时
    ERR_REPORT_REJECTED                         = -404,    // 表示更新设备影子文档被拒绝
    ERR_GET_TIMEOUT                             = -405,    // 表示拉取设备影子文档超时
    ERR_GET_REJECTED                            = -406,    // 表示拉取设备影子文档被拒绝
	ERR_ACTION_EXIST                            = -407,    // acion already exist
    ERR_ACTION_NOT_EXIST                        = -408,    // acion not exist
} eResault;


#ifdef __cplusplus
}
#endif

#endif //_EXPORT_ERROR_H_