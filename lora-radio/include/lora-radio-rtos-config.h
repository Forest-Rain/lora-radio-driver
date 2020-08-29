/*!
 * \file      lora-radio-rtos-config.h
 *
 * \brief     adapter to different RTOS implementation
 *
 * \copyright SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */
 
#ifndef __LORA_RADIO_RTOS_CONFIG_H_
#define __LORA_RADIO_RTOS_CONFIG_H_

#define USING_RTOS_RT_THREAD
//#define USING_RTOS_RT_THREAD_NANO

#ifdef USING_RTOS_RT_THREAD
#elif defined USING_RTOS_RT_THREAD_NANO
#else
    #error "Please Choose your RTOS setup!"
#endif

#ifdef USING_RTOS_RT_THREAD
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_spi.h"
#include "drv_gpio.h"

#define LOG_LVL_ASSERT                 0
#define LOG_LVL_ERROR                  3
#define LOG_LVL_WARNING                4
#define LOG_LVL_INFO                   6
#define LOG_LVL_DBG                    7

#elif defined USING_RTOS_RT_THREAD_NANO
#include <rtthread.h>

#endif


#endif // end of __LORA_RADIO_RTOS_CONFIG_H_
