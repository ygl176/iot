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
    ESP8266_UNINITIALIZED = 0,  //未初始化
    ESP8266_INITIALIZED,        //初始化
    ESP8266_BUSY,               //忙
}esp8266_status;

typedef enum
{
	STA = 0,
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
	eTCP,
	eUDP,
}eNetPro;

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
    GET_FAIL = 0,       //获取状态失败
    GOT_IP = 2,         //获得IP
    CONNECT,            //建立连接
    N_CONNECT,          //失去连接
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
    uint8_t* buf;
    uint16_t buf_size;

}response;

typedef response *response_t;

typedef struct
{
    esp8266_status status;  //esp8266外设状态

    void *recv_que;         //消息处理缓冲区

    void *lock;             //互斥信号量

    bool resp_notice;       //接收到消息
    response_t resp;      //主动请求响应结果，同一时间只能有一个主动请求

    // ParserFunc parse;
}bsp_esp8266;


typedef bsp_esp8266 * tbsp_esp8266;

/**
 * @brief 主动请求响应空间申请
 * 
 * @param buf_len 
 * @return response_t 
 */
response_t resp_malloc(uint32_t buf_len);

/**
 * @brief 主动请求响应空间释放
 * 
 * @param resp 
 */
void resp_release(response_t resp);

/**
 * @brief 获取 esp 设备抽象指针
 * 
 * @return tbsp_esp8266 
 */
tbsp_esp8266 dev_esp_get();

/**
 * @brief 初始化 esp 外设抽象
 * 
 * @param esp 
 * @return true 
 * @return false 
 */
bool dev_esp_init(tbsp_esp8266 p_esp);

/**
 * @brief ESP8266 模块初始化
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_Init();

/**
 * @brief 模块复位
 * 
 */
void ESP8266_Rst();

/**
 * @brief 发送 AT 指令
 * 
 * @param cmd 指令字符串
 * @param reply  期望响应字符串
 * @param wait_ms 最大等待时间
 * @return at_response 返回响应内容，返回结果字符串为空时报错
 */
response ESP8266_Cmd(response_t resp, char *cmd, uint32_t wait_ms);

/**
 * @brief AT 指令测试
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_AT_Test();

/**
 * @brief ESP8266 模式设置
 * 
 * @param eMode AP, STA, AP/STA
 * @return true 
 * @return false 
 */
bool ESP8266_Mode_Set(eESP_Mode eMode);

/**
 * @brief 连接指定Wifi
 * 
 * @param pSSID 
 * @param pPassWord 
 * @return true 
 * @return false 
 */
bool ESP8266_JoinAP(char *pSSID, char *pPassWord);

/**
 * @brief 使能DHCP
 * 
 */
// bool ESP8266_DHCP_CUR(void);

/**
 * @brief 设置 ESP8266 AP模式状态
 * 
 * @param pSSID 
 * @param pPassWord 
 * @param eMode 加密格式
 * @return true 
 * @return false 
 */
bool ESP8266_BuildAP(char *pSSID, char *pPassWord, eAP_PsdMode eMode);

/**
 * @brief 连接服务器
 * 
 * @param eNet 连接方式
 * @param ip 服务器ip地址
 * @param ComNum 服务器端口
 * @param id 本地端口
 * @return true 
 * @return false 
 */
bool ESP8266_Link_Server(eNetPro eNet, char *ip, char *ComNum, eID_NO id);

/**
 * @brief 获取wifi连接状态
 * 
 * @return eAP_Link_Sta 
 */
eAP_Link_Sta ESP8266_GET_LinkStatus();

/**
 * @brief 获取端口连接状态
 * 
 * @return uint8_t 
 * 低5位有效，每一位表示一个端口，连接置 1，否则置 0
 */
uint8_t ESP8266_Get_IdLinkStatus();

/**
 * @brief 请求 ESP8266 的 AP IP地址
 * 
 * @param pApIp 
 * @param buf_len 
 * @return true 
 * @return false 
 */
bool ESP8266_Inquire_ApIp(char *pApIp, uint8_t buf_len);

/**
 * @brief 使能透传模式
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_UnvarnishSend();

/**
 * @brief 关闭透传模式
 * 
 */
void ESP8266_ExitUnvarnishSend();

/**
 * @brief ESP8266向服务器发送字符串
 * 
 * @param pStr 
 * @param StrLen 
 * @param Id 本地端口
 * @return true 
 * @return false 
 */
bool ESP8266_SendStr(char *pStr, uint32_t StrLen, eID_NO Id);

/**
 * @brief 从服务器接收字符串
 * 
 * @return char* 
 */
char* ESP8266_RecStr();


void esp_parse(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_AT_ESP8266_H__
