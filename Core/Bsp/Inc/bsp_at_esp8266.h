/**
 * @file bsp_esp8266.h
 * @author ygl
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __BSP_AT_ESP8266_H__
#define __BSP_AT_ESP8266_H__

#include "stdbool.h"
#include "stdint.h"
#include "ringbuff.h" 
#include "export_err.h"


#ifdef __cplusplus
extern "C" {
#endif

#define AT_FRAME_VERSION            ""


#define AT_BUFF_LEN                     1024
#define AT_GET_CHAR_TIMEOUT_MS          5000
#define AT_CMD_TIMEOUT_MS               10000
#define AT_CMD_RESPONSE_INTERVAL_MS        100

typedef void (*ParserFunc)(void *userContex);

typedef enum
{
    ESP8266_UNINITIALIZED = 0,  //δ��ʼ��
    ESP8266_INITIALIZED,        //��ʼ��
    ESP8266_BUSY,               //æ
}esp8266_status;

typedef enum
{
	STA = 1,
  	AP,
  	STA_AP  
}eESP_Mode;

typedef enum
{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
}eAP_PsdMode;

typedef enum
{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
}eID_NO;

typedef enum
{
    GET_FAIL = 0,       //��ȡ״̬ʧ��
    GOT_IP = 2,         //���IP
    AP_CONNECT,            //��������
    N_CONNECT,          //ʧȥ����
}eAP_Link_Sta;

typedef enum
{
    AT_RESP_OK = 0,
    AT_RESP_ERROR = -1,
    AT_RESP_TIMEOUT = -2,
    AT_RESP_BUFF_FULL = -3,
}at_resp_status_t;

typedef struct
{
    uint16_t buf_size;  //������
    uint16_t buf_num;   //�����ַ�����
    char* buf;
}response;

typedef response *response_t;

typedef struct
{
    esp8266_status status;      //esp8266����״̬

    ring_buff_t p_ring_buff;    //���ڻ�״������

    void *sema_rx;              //���ڻ������ַ������ź���
    char *recv;                 //��Ϣ��������
    uint32_t recv_len;
    uint32_t cur_recv_len;
    bool recv_notice;           //������Ϣָʾ

    void *lock;                 //�����ź���

    bool resp_notice;           //���յ���Ϣ
    bool resp_status;           //��Ӧ���
    response_t resp;            //����������Ӧ�����ͬһʱ��ֻ����һ����������

    // ParserFunc parse;
}bsp_esp8266;


typedef bsp_esp8266 * tbsp_esp8266;

/**
 * @brief ����������Ӧ�ռ�����
 * 
 * @param buf_len 
 * @return response_t 
 */
response_t resp_malloc(uint32_t buf_len);

/**
 * @brief ����������Ӧ�ռ��ͷ�
 * 
 * @param resp 
 */
void resp_release(response_t resp);

/**
 * @brief ��ȡ esp �豸����ָ��
 * 
 * @return tbsp_esp8266 
 */
tbsp_esp8266 dev_esp_get();

/**
 * @brief ��ʼ�� esp �������
 * 
 * @param esp 
 * @return true 
 * @return false 
 */
bool dev_esp_init(tbsp_esp8266 p_esp);

/**
 * @brief ESP8266 ģ���ʼ��
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_Init();

/**
 * @brief ģ�鸴λ
 * 
 */
void ESP8266_Rst();

/**
 * @brief ���� AT ָ��
 * 
 * @param cmd ָ���ַ���
 * @param wait_ms ���ȴ�ʱ��
 * @param fmt ��ӡ����
 * @return
 */
bool ESP8266_Cmd(response_t resp, uint32_t wait_ms, const char *fmt, ...);

/**
 * @brief AT ָ�����
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_AT_Test();

/**
 * @brief ���ڻ�������
 * 
 * @param mode 
 * 1 ��������
 * 0 �رջ���
 * @return true
 * @return false
 */
bool ESP8266_ATE(uint8_t mode);

/**
 * @brief ESP8266 ģʽ����
 * 
 * @param eMode AP, STA, AP/STA
 * @return true 
 * @return false 
 */
bool ESP8266_Mode_Set(eESP_Mode eMode);

/**
 * @brief ����ָ��Wifi
 * 
 * @param pSSID 
 * @param pPassWord 
 * @return true 
 * @return false 
 */
bool ESP8266_JoinAP(char *pSSID, char *pPassWord);

/**
 * @brief ʹ��DHCP
 * 
 */
// bool ESP8266_DHCP_CUR(void);

/**
 * @brief ���� ESP8266 APģʽ״̬
 * 
 * @param pSSID 
 * @param pPassWord 
 * @param eMode ���ܸ�ʽ
 * @return true 
 * @return false 
 */
bool ESP8266_BuildAP(char *pSSID, char *pPassWord, eAP_PsdMode eMode);

/**
 * @brief ���ӷ�����
 * 
 * @param eNet ���ӷ�ʽ
 * @param ip ������ip��ַ
 * @param ComNum �������˿�
 * @param id ���ض˿�
 * @return true 
 * @return false 
 */
bool ESP8266_Link_Server(char* net, char *ip, char *ComNum, eID_NO id);

/**
 * @brief ��ȡ���˿�����״̬
 * 
 * @return eAP_Link_Sta 
 */
eAP_Link_Sta ESP8266_GET_LinkStatus();

/**
 * @brief ��ȡ��˿�����״̬
 * 
 * @return uint8_t 
 * ��5λ��Ч��ÿһλ��ʾһ���˿ڣ������� 1�������� 0
 */
uint16_t ESP8266_Get_IdLinkStatus();

/**
 * @brief ���� ESP8266 �� AP IP��ַ
 * 
 * @param pApIp 
 * @param buf_len 
 * @return true 
 * @return false 
 */
bool ESP8266_Inquire_ApIp(char *pApIp, uint8_t buf_len);

/**
 * @brief ʹ��͸��ģʽ
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_UnvarnishSend();

/**
 * @brief �ر�͸��ģʽ
 * 
 */
void ESP8266_ExitUnvarnishSend();

/**
 * @brief ESP8266������������ַ���
 * 
 * @param pStr 
 * @param StrLen 
 * @param Id ���ض˿�
 * @return true 
 * @return false 
 */
bool ESP8266_SendStr(char *pStr, uint32_t StrLen, eID_NO Id);

/**
 * @brief �ӷ����������ַ���
 * 
 * @return char* 
 */
char* ESP8266_RecStr();

/**
 * @brief esp������Ϣ����
 * 
 */
void esp_parse(void *arg);

#ifdef __cplusplus
}
#endif

#endif //__BSP_AT_ESP8266_H__
