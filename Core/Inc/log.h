/**
 * @file bsp_log.h
 * @author ygl
 * @brief 
 * @version 0.1
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// 日志输出等级
typedef enum {
    LOG_DEBUG,  //调试
    LOG_INFO,   //通知
    LOG_WARN,   //警告
    LOG_ERROR   //错误
} LOG_LEVEL;


// 全局日志级别输出标志, 只输出小于或等于该等级的日志信息
// extern LOG_LEVEL g_log_level;


/**
 * @brief 设置全局日志输出级别标志
 * 
 * @param level 
 */
void Log_Set_Level(LOG_LEVEL level);


/**
 * @brief 获取当前日志等级
 * 
 * @return LOG_LEVEL 
 */
LOG_LEVEL Log_Get_Level(void);


/**
 * @brief 日志打印函数，默认打印到标准输出
 *
 * @param file 源文件名
 * @param func 函数名
 * @param line 行号
 * @param level 日志等级
 */
void Log_writter(const char *file, const char *func, const int line, const int level, const char *fmt, ...);


#define Log_d(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_DEBUG, args)
#define Log_i(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_INFO, args)
#define Log_w(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_WARN, args)
#define Log_e(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_ERROR, args)


#ifdef __cplusplus
}
#endif

#endif //_LOG_H_