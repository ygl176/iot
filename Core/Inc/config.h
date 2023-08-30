/**
 * @file config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define AUTH_MODE_KEY

// #define EVENT_POST_ENABLED
// #define ACTION_ENABLED
#define DEBUG_DEV_INFO_USED 

//使能系统
#define OS_USED
//系统功能选择
#ifdef OS_USED
#define MUTEX
#define QUEUE
#endif

//使能日志输出
#define LOG

#define MODULE_TYPE_ESP8266


#define AT_CMD_MAX_LEN                 1024
#define RING_BUFF_LEN         		   AT_CMD_MAX_LEN	 //uart ring buffer len
#define ESP_BUFF_LEN                   1024


#endif //__CONFIG_H__
