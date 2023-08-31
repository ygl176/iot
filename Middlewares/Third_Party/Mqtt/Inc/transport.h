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

#define MQTT_USART      &huart1

extern __IO uint8_t MQTT_RX_FLAG;
extern uint8_t MQTT_RX_BUF[512];
extern __IO uint16_t MQTT_RX_COUNT;

/**
 * @brief mqtt报文发送函数
 * 
 * @param buf 指令字符
 * @param buflen 发送长度
 * @param resp 返回消息
 * @return bool 
 */
bool transport_sendPacketBuffer(unsigned char* buf, int buflen, response_t resp, uint32_t wait_ms);
int transport_getdata(unsigned char* buf, int count);
int transport_getdatanb(void *sck, unsigned char* buf, int count);
int transport_open(char* host, int port);
int transport_close(int sock);

#endif
