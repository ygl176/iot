/**
 * @file dev_info.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "string.h"
#include "dev_info.h"
#include "stddef.h"

#ifdef  DEBUG_DEV_INFO_USED

/* ��Ʒ����, ���ƶ�ͬ���豸״̬ʱ��Ҫ  */
static char sg_product_id[MAX_SIZE_OF_PRODUCT_ID + 1]	 = "PRODUCT_ID";
/* �豸����, ���ƶ�ͬ���豸״̬ʱ��Ҫ */
static char sg_device_name[MAX_SIZE_OF_DEVICE_NAME + 1]  = "YOUR_DEV_NAME";

#ifdef DEV_DYN_REG_ENABLED
/* ��Ʒ��Կ, ��ʹ�ܶ�̬ע�Ṧ�ܣ�����̨���ɣ��������ʹ�ܣ����ø�ֵ  */
static char sg_product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1]  = "YOUR_PRODUCT_SECRET";
#endif

#ifdef AUTH_MODE_CERT
/* �ͻ���֤���ļ���  �ǶԳƼ���ʹ��, TLS ֤����֤��ʽ*/
static char sg_device_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1]      = "YOUR_DEVICE_NAME_cert.crt";
/* �ͻ���˽Կ�ļ��� �ǶԳƼ���ʹ��, TLS ֤����֤��ʽ*/
static char sg_device_privatekey_file_name[MAX_SIZE_OF_DEVICE_KEY_FILE_NAME + 1] = "YOUR_DEVICE_NAME_private.key";
#else
/* �豸��Կ, TLS PSK��֤��ʽ*/
static char sg_device_secret[MAX_SIZE_OF_DEVICE_SERC + 1] = "YOUR_IOT_PSK";
#endif



bool Dev_Set_Info(DeviceInfo *pdevInfo)
{
    if(pdevInfo == NULL)
        return false;

    #ifdef DEBUG_DEV_INFO_USED
	memset(sg_product_id, '\0', MAX_SIZE_OF_PRODUCT_ID);	
	memset(sg_device_name, '\0', MAX_SIZE_OF_DEVICE_NAME);
	
	strncpy(sg_product_id, pdevInfo->product_id, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(sg_device_name, pdevInfo->device_name, MAX_SIZE_OF_DEVICE_NAME);

#ifdef DEV_DYN_REG_ENABLED
	memset(sg_product_secret, '\0', MAX_SIZE_OF_PRODUCT_SECRET);
	strncpy(sg_product_secret, pdevInfo->product_secret, MAX_SIZE_OF_PRODUCT_KEY);
#endif
	
#ifdef 	AUTH_MODE_CERT
	memset(sg_device_cert_file_name, '\0', MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	memset(sg_device_privatekey_file_name, '\0', MAX_SIZE_OF_DEVICE_KEY_FILE_NAME);
	
	strncpy(sg_device_cert_file_name, pdevInfo->devCertFileName, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	strncpy(sg_device_privatekey_file_name, pdevInfo->devPrivateKeyFileName, MAX_SIZE_OF_DEVICE_KEY_FILE_NAME);
#else
	memset(sg_device_secret, '\0', MAX_SIZE_OF_DEVICE_SERC);
	strncpy(sg_device_secret, pdevInfo->devSerc, MAX_SIZE_OF_DEVICE_SERC);
#endif
		
#else
	 Log_e("HAL_SetDevInfo is not implement");
	 (void)pdevInfo; //eliminate compile warning

	 return false;

#endif

	return true;
}


bool Dev_Get_Info(DeviceInfo *pdevInfo)
{
	if(NULL == pdevInfo){
		return false;
	}
	
	memset((char *)pdevInfo, '\0', sizeof(DeviceInfo));	
	
#ifdef DEBUG_DEV_INFO_USED	

	strncpy(pdevInfo->product_id, sg_product_id, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(pdevInfo->device_name, sg_device_name, MAX_SIZE_OF_DEVICE_NAME);
	
#ifdef DEV_DYN_REG_ENABLED
	memset(pdevInfo->product_secret, '\0', MAX_SIZE_OF_PRODUCT_SECRET);
	strncpy(pdevInfo->product_secret, sg_product_secret, MAX_SIZE_OF_PRODUCT_SECRET);
#endif	
	
#ifdef 	AUTH_MODE_CERT
	strncpy(pdevInfo->devCertFileName, sg_device_cert_file_name, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	strncpy(pdevInfo->devPrivateKeyFileName, sg_device_privatekey_file_name, MAX_SIZE_OF_DEVICE_KEY_FILE_NAME);
#else
	strncpy(pdevInfo->devSerc, sg_device_secret, MAX_SIZE_OF_DEVICE_SERC);
#endif 

#else
   Log_e("HAL_GetDevInfo is not implement");

   return false;
#endif

	return true;
}

#endif //DEBUG_DEV_INFO_USED

