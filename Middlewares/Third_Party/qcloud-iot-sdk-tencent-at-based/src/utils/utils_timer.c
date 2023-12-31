/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif
    
#include "utils_timer.h"
#include "stm32f1xx_hal.h"

    
bool expired(Timer *timer) {
    uint32_t now_ts;
	
	now_ts	= HAL_GetTick();

    return (now_ts > timer->end_time)?true:false;
}

void countdown_ms(Timer *timer, unsigned int timeout_ms) {
    timer->end_time = HAL_GetTick();
    timer->end_time += timeout_ms;	
}

void countdown(Timer *timer, unsigned int timeout) {
    timer->end_time = HAL_GetTick();
	timer->end_time += timeout*1000;
}

int left_ms(Timer *timer) {
    return (int)(timer->end_time - HAL_GetTick());
}

void InitTimer(Timer *timer) {
    timer->end_time = 0;
}
    
#ifdef __cplusplus
}
#endif
