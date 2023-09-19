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
 * 40位 通讯数据格式
 * 湿度高8位
 * 湿度低8位
 * 温度高8位    有符号8位数据   
 * 温度低8位
 * 校验位 (前4个8位数据之和)
 */

/**
 * @brief DH11芯片初始化
 * 
 */
void dh11_init();


/**
 * @brief dh11芯片复位
 * 
 */
bool dh11_rst();

/**
 * @brief 获取温湿度数据
 * 
 * @param temp 温度数据指针
 * @param humi 湿度数据指针
 * @return true 获取成功
 * @return false 获取失败
 */
bool dh11_get_data(float *temp, float *humi);

#endif /* INC_BSP_DH11_H_ */
