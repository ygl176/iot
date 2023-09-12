/**
 * @file dev_info.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __DEV_INFO_H__
#define __DEV_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "config.h"
#include "mqtt.h"
#include "stdbool.h"


typedef struct {
	char	product_id[MAX_SIZE_OF_PRODUCT_ID + 1];
	char 	device_name[MAX_SIZE_OF_DEVICE_NAME + 1];
	char	client_id[MAX_SIZE_OF_CLIENT_ID + 1];
	
#ifdef AUTH_MODE_CERT
	char  	devCertFileName[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1];
	char 	devPrivateKeyFileName[MAX_SIZE_OF_DEVICE_KEY_FILE_NAME + 1];
#else
	char	devSerc[MAX_SIZE_OF_DEVICE_SERC + 1];
#endif

#ifdef DEV_DYN_REG_ENABLED
	char	product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1];
#endif  
} DeviceInfo;


bool Dev_Set_Info(DeviceInfo *pdevInfo);
bool Dev_Get_Info(DeviceInfo *pdevInfo);

#ifdef __cplusplus
}
#endif

#endif //__DEV_INFO_H__