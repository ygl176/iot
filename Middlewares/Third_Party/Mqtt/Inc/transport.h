/**
 * @file transport.h
 * @author
 * @brief 
 * @version 0.1
 * @date 2023-08-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __TRANSPORT_H
#define __TRANSPORT_H

#include "main.h"
#include "bsp_at_esp8266.h"

#define MQTT_USART      &huart1

response_t mqtt_get_resp();
/**
 * @brief mqtt报文发送函数
 * 
 * @param buf 指令字符
 * @param buflen 发送长度
 * @param resp 返回消息
 * @return bool 
 */
bool transport_sendPacketBuffer(unsigned char* buf, int buflen, response_t resp, uint16_t wait_flag, uint32_t wait_time);
int transport_getdata(unsigned char* buf, int count);
int transport_getdatanb(void *sck, unsigned char* buf, int count);
int transport_open(char* host, int port);
int transport_close(int sock);

#endif
