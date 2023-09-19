/*
 * bsp_dh11.h
 *
 *  Created on: Sep 13, 2023
 *      Author: 18400
 */

#ifndef INC_BSP_DH11_H_
#define INC_BSP_DH11_H_

#include "stdbool.h"
/**
 * 40λ ͨѶ���ݸ�ʽ
 * ʪ�ȸ�8λ
 * ʪ�ȵ�8λ
 * �¶ȸ�8λ    �з���8λ����   
 * �¶ȵ�8λ
 * У��λ (ǰ4��8λ����֮��)
 */

/**
 * @brief DH11оƬ��ʼ��
 * 
 */
void dh11_init();


/**
 * @brief dh11оƬ��λ
 * 
 */
bool dh11_rst();

/**
 * @brief ��ȡ��ʪ������
 * 
 * @param temp �¶�����ָ��
 * @param humi ʪ������ָ��
 * @return true ��ȡ�ɹ�
 * @return false ��ȡʧ��
 */
bool dh11_get_data(float *temp, float *humi);

#endif /* INC_BSP_DH11_H_ */
