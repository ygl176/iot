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
    RET_SUCCESS   						        = 0,      // ��ʾ�ɹ�����
    ERR_FAILURE   						        = -100,   // ��ʾʧ�ܷ���
    ERR_INVAL     						        = -101,   // ��ʾ������Ч����
    ERR_NULL       						        = -102,   // ��ʾ��ָ��
    ERR_MALLOC     						        = -103,   // ��ʾ�ڴ�����ʧ��
    ERR_SEND_DATA   				            = -104,   // ��ʾAT���ڷ���ʧ��
   

	ERR_TIMEOUT	  						        = -201,	   //��Ӧ��ʱ	
	ERR_RESP_NULL  						        = -202,	   //��Ӧ����ָ����Ч
	ERR_EXCEED_MAX_TOPICS 				        = -203,    //�������topic�ɶ�����
	ERR_MQTT_NO_CONN				            = -204,    //MQTTδ����


    ERR_JSON_PARSE                              = -301,    // ��ʾJSON��������
    ERR_JSON_BUFFER_TRUNCATED                   = -302,    // ��ʾJSON�ĵ��ᱻ�ض�
    ERR_JSON_BUFFER_TOO_SMALL                   = -303,    // ��ʾ�洢JSON�ĵ��Ļ�����̫С
    ERR_JSON                                    = -304,    // ��ʾJSON�ĵ����ɴ���
    ERR_MAX_JSON_TOKEN                          = -305,    // ��ʾ����JSON�ĵ��е����Token��
    ERR_MAX_APPENDING_REQUEST                   = -306,    // ��ʾ����ͬʱ�����ĵ�����
    ERR_MAX_TOPIC_LENGTH                        = -307,    // ��ʾ�����涨����topic����


	ERR_PROPERTY_EXIST                          = -401,    // ��ʾע��������Ѿ�����
    ERR_PROPERTY_NOT_EXIST                      = -402,    // ��ʾע������Բ�����
    ERR_REPORT_TIMEOUT                          = -403,    // ��ʾ�����豸Ӱ���ĵ���ʱ
    ERR_REPORT_REJECTED                         = -404,    // ��ʾ�����豸Ӱ���ĵ����ܾ�
    ERR_GET_TIMEOUT                             = -405,    // ��ʾ��ȡ�豸Ӱ���ĵ���ʱ
    ERR_GET_REJECTED                            = -406,    // ��ʾ��ȡ�豸Ӱ���ĵ����ܾ�
	ERR_ACTION_EXIST                            = -407,    // acion already exist
    ERR_ACTION_NOT_EXIST                        = -408,    // acion not exist
} eResault;


#ifdef __cplusplus
}
#endif

#endif //_EXPORT_ERROR_H_