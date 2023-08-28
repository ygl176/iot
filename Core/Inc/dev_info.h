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
#include "stdbool.h"

/* 设备ID的最大长度, 必须保持唯一 */
#define 	MAX_SIZE_OF_CLIENT_ID                                   (80)

/* 产品名称的最大长度 */
#define MAX_SIZE_OF_PRODUCT_ID                                    	(10)

/* 产品密钥的最大长度 ，动态设备注册需要*/
#define MAX_SIZE_OF_PRODUCT_SECRET                                  (48)


/* 设备ID的最大长度 */
#define MAX_SIZE_OF_DEVICE_NAME                                     (64)

/* psk最大长度 */
#define MAX_SIZE_OF_DEVICE_SERC  	 								(24)


/* 设备证书文件名的最大长度 */
#define MAX_SIZE_OF_DEVICE_CERT_FILE_NAME                           (128)

/* 设备私钥文件名的最大长度 */
#define MAX_SIZE_OF_DEVICE_KEY_FILE_NAME                            (128)

/* 云端保留主题的最大长度 */
#define MAX_SIZE_OF_CLOUD_TOPIC            ((MAX_SIZE_OF_DEVICE_NAME) + (MAX_SIZE_OF_PRODUCT_ID) + 64)


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