/**
 * @file bsp_log.c
 * @author ygl
 * @brief 
 * @version 0.1
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include "usart.h"
#include "common.h"
#include <string.h>

//日志输出串口
#define LOG_UART        huart2
//日志输出缓冲区长度
#define LOG_BUF_LEN     1024

//高于 g_log_level 的日志才可以输出
static LOG_LEVEL g_log_level = LOG_DEBUG;
static bool log_lock = false;



static char *level_str[] = {
    "DBG", "INF", "WRN", "ERR"
};



static const char *_get_filename(const char *p)
{
    char ch = '/';
    const char *q = strrchr(p, ch);

    if(q == NULL)
        q = p;
    else
        q++;
    return q;
}



void Log_Set_Level(LOG_LEVEL level)
{
    g_log_level = level;
}


LOG_LEVEL Log_Get_Level(void)
{
    return g_log_level;
}



void Log_writter(const char *file, const char *func, const int line, const int level, const char *fmt, ...)
{
    if(level < g_log_level) 
        return;

    if(log_lock) 
    	return;
    else
        log_lock = true;

    const char *file_name = _get_filename(file);

    usart_printf(&LOG_UART,"%s|%s|%s(%d): ", level_str[level], file_name, func, line);

    va_list ap;
	char bufs[LOG_BUF_LEN];

	va_start(ap, fmt);
	vsnprintf(bufs, sizeof(bufs), fmt, ap);
	va_end(ap);

    usart_printf(&LOG_UART, "\r\n");

    /** printf串口输出日志 **/
    // printf("%s|%s|%s(%d): ", level_str[level], file_name, func, line);

    // va_list ap;
    // va_start(ap, fmt);
    // vprintf(fmt, ap);
    // va_end(ap);

    // printf("\r\n");

    log_lock = false;
}



#ifdef __cplusplus
}
#endif
