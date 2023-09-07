/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/
#include "log.h"
#include "transport.h"
#include "cmsis_os.h"
#include "usart.h"
#include <string.h>
#include "common.h"


/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
static response mqtt_resp;

response_t mqtt_get_resp()
{
	return &mqtt_resp;
}

bool transport_sendPacketBuffer(unsigned char* buf, int buflen, response_t resp, uint32_t wait_ms)
{
	bool ret = true;
	uint32_t target_time = HAL_GetTick() + wait_ms;
	tbsp_esp8266 p_esp = dev_esp_get();

	p_esp->resp = resp;
	p_esp->resp_notice = false;

	if(HAL_UART_Transmit(MQTT_USART, (uint8_t*)buf, buflen, HAL_MAX_DELAY) != HAL_OK)
	{
		Log_e("mqtt cmd send failed");
		return false;
	}

	if(wait_ms == 0 && resp == NULL) return true;	//不需要响应报文

    while(p_esp->resp_notice != true && HAL_GetTick() <= target_time)	//物理层协议不要等待，而是加入消息链表
    {
        // HAL_Delay(10);
    }

    if(HAL_GetTick() > target_time) //超时
    {
        Log_e("esp cmd(%s) timeout %dms", buf, wait_ms);

        p_esp->resp_status = false;
        ret = false;
    }

	p_esp->resp = NULL;
	p_esp->resp_notice = false;

	return ret;
}


int transport_getdata(unsigned char* buf, int count)
{
	response_t resp = mqtt_get_resp();
	
	if(count > resp->buf_num)
		return 0;

	uint16_t delay_cnt = 0;
	static uint16_t read_count = 0;

	memcpy(buf, resp->buf + read_count, count);

	read_count += count;

	if(read_count >= resp->buf_num)
	{
		resp->buf_num = 0;
		read_count = 0;
	}

	return count;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	return 0;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
	return 0;
}

int transport_close(int sock)
{
	return 0;
}
