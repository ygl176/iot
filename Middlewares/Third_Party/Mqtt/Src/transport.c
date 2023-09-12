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
#include "mqtt.h"
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
static response mqtt_resp; //mqtt协议接收缓冲区

response_t mqtt_get_resp()
{
	return &mqtt_resp;
}

bool transport_sendPacketBuffer(unsigned char* buf, int buflen, response_t resp, uint16_t wait_flag, uint32_t wait_time)
{
	bool ret = true;
	uint32_t target_time = HAL_GetTick() + wait_time;
	tbsp_esp8266 p_esp = dev_esp_get();

	HAL_MutexLock(p_esp->lock);

	p_esp->resp = resp;

	mqtt_flag_clear(wait_flag);

	if(HAL_UART_Transmit(MQTT_USART, (uint8_t*)buf, buflen, HAL_MAX_DELAY) != HAL_OK)
	{
		Log_e("mqtt cmd send failed");
		return false;
	}

	if(!wait_flag)	//无需等待标志位
		goto EXIT;

	while((!mqtt_flag_get(wait_flag)) && HAL_GetTick() < target_time);  //规定时间内等待目的标志置位
    if(HAL_GetTick() >= target_time)
    {
        Log_e("mqtt reply timeout");
        ret = false;
    }

	mqtt_flag_clear(wait_flag);

	EXIT:

	p_esp->resp = NULL;

	HAL_MutexUnlock(p_esp->lock);

	return ret;
}


int transport_getdata(unsigned char* buf, int count)
{
	response_t resp = mqtt_get_resp();
	
	if(count > resp->buf_num)
		return 0;

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
