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
    ESP8266_UNINITIALIZED = 0,  //未初始化
    ESP8266_INITIALIZED,        //初始化
    ESP8266_BUSY,               //忙
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
    GET_FAIL = 0,       //获取状态失败
    GOT_IP = 2,         //获得IP
    AP_CONNECT,            //建立连接
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
    uint16_t buf_size;  //缓冲区
    uint16_t buf_num;   //接收字符数量
    char* buf;
}response;

typedef response *response_t;

typedef struct
{
    esp8266_status status;      //esp8266外设状态

    ring_buff_t p_ring_buff;    //串口环状缓冲区

    void *sema_rx;              //串口缓冲区字符接收信号量
    char *recv;                 //消息处理缓冲区
    uint32_t recv_len;
    uint32_t cur_recv_len;
    bool recv_notice;           //串口消息指示

    void *lock;                 //互斥信号量

    bool resp_notice;           //接收到消息
    bool resp_status;           //响应结果
    response_t resp;            //主动请求响应结果，同一时间只能有一个主动请求

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
 * @param wait_ms 最大等待时间
 * @param fmt 打印内容
 * @return
 */
bool ESP8266_Cmd(response_t resp, uint32_t wait_ms, const char *fmt, ...);

/**
 * @brief AT 指令测试
 * 
 * @return true 
 * @return false 
 */
bool ESP8266_AT_Test();

/**
 * @brief 串口回显设置
 * 
 * @param mode 
 * 1 开启回显
 * 0 关闭回显
 * @return true
 * @return false
 */
bool ESP8266_ATE(uint8_t mode);

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
bool ESP8266_Link_Server(char* net, char *ip, char *ComNum, eID_NO id);

/**
 * @brief 获取单端口连接状态
 * 
 * @return eAP_Link_Sta 
 */
eAP_Link_Sta ESP8266_GET_LinkStatus();

/**
 * @brief 获取多端口连接状态
 * 
 * @return uint8_t 
 * 低5位有效，每一位表示一个端口，连接置 1，否则置 0
 */
uint16_t ESP8266_Get_IdLinkStatus();

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

/**
 * @brief esp接收消息处理
 * 
 */
void esp_parse(void *arg);

#ifdef __cplusplus
}
#endif

#endif //__BSP_AT_ESP8266_H__
