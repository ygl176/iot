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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// ��־����ȼ�
typedef enum {
    LOG_DEBUG,  //����
    LOG_INFO,   //֪ͨ
    LOG_WARN,   //����
    LOG_ERROR   //����
} LOG_LEVEL;


// ȫ����־���������־, ֻ���С�ڻ���ڸõȼ�����־��Ϣ
// extern LOG_LEVEL g_log_level;


/**
 * @brief ����ȫ����־��������־
 * 
 * @param level 
 */
void Log_Set_Level(LOG_LEVEL level);


/**
 * @brief ��ȡ��ǰ��־�ȼ�
 * 
 * @return LOG_LEVEL 
 */
LOG_LEVEL Log_Get_Level(void);


/**
 * @brief ��־��ӡ������Ĭ�ϴ�ӡ����׼���
 *
 * @param file Դ�ļ���
 * @param func ������
 * @param line �к�
 * @param level ��־�ȼ�
 */
void Log_writter(const char *file, const char *func, const int line, const int level, const char *fmt, ...);


//�Զ��� log ʹ��
#ifdef LOG

#define Log_d(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_DEBUG, args)
#define Log_i(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_INFO, args)
#define Log_w(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_WARN, args)
#define Log_e(args...) Log_writter(__FILE__, __FUNCTION__, __LINE__, LOG_ERROR, args)

#else

#define Log_d(args...)
#define Log_i(args...)
#define Log_w(args...)
#define Log_e(args...)

#endif  //LOG

//#define DEBUG_EABLE
#ifdef DEBUG_EABLE
	#define IOT_FUNC_ENTRY    \
		{\
		printf("FUNC_ENTRY:   %s L#%d \n", __FUNCTION__, __LINE__);  \
		}
	#define IOT_FUNC_EXIT    \
		{\
		printf("FUNC_EXIT:   %s L#%d \n", __FUNCTION__, __LINE__);  \
		return;\
		}
	#define IOT_FUNC_EXIT_RC(x)    \
		{\
		printf("FUNC_EXIT:   %s L#%d Return Code : %ld \n", __FUNCTION__, __LINE__, (long)(x));  \
		return x; \
		}
#else
	#define IOT_FUNC_ENTRY
	#define IOT_FUNC_EXIT 			\
		{\
			return;\
		}
	#define IOT_FUNC_EXIT_RC(x)     \
		{\
			return x; \
		}
#endif

#ifdef __cplusplus
}
#endif

#endif //_LOG_H_