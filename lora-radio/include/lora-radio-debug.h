/*!
 * \file      lora-radio-debug.h
 *
 * \brief     control all LoRa Radio Driver debug features.
 *
 * \copyright SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */
 
#ifndef __LORA_RADIO_DEBUG_H__
#define __LORA_RADIO_DEBUG_H__

#if ( defined USING_LORA_RADIO_ON_RTOS_RT_THREAD ) || ( defined USING_LORA_RADIO_ON_RTOS_RT_THREAD_NANO )
#ifdef RT_USING_ULOG
#include <rtdbg.h>
#include <ulog.h> 
#else

#endif
#elif defined USING_RTOS_CONTIKI_NG
#include "log.h"
#endif


/* Using this macro to control all LoRa Radio Driver debug features. */
#ifdef USING_LORA_RADIO_DEBUG

/* Turn on some of these (set to non-zero) to debug LORa Radio */

/* applicaiton */
#ifndef LR_DBG_APP
#define LR_DBG_APP                          0
#endif

/* API interface */
#ifndef LR_DBG_INTERFACE
#define LR_DBG_INTERFACE                    0
#endif

/*lora chip driver, eg sx126x.c¡¢sx27x.c*/
#ifndef LR_DBG_CHIP
#define LR_DBG_CHIP                         0
#endif

/* spi driver ,eg lora-spi.c*/
#ifndef LR_DBG_SPI
#define LR_DBG_SPI                          0
#endif

#if ( defined USING_LORA_RADIO_ON_RTOS_RT_THREAD ) || ( defined USING_LORA_RADIO_ON_RTOS_RT_THREAD_NANO )
#if defined RT_USING_ULOG || defined RT_DEBUG

#else
    #define ulog_output(level,...)  rt_kprintf(__VA_ARGS__)
#endif

#define LORA_RADIO_DEBUG_LOG(type, level, ...)                                 \
do                                                                            \
{                                                                             \
    if (type)                                                                 \
    {                                                                         \
        ulog_output(level, LOG_TAG, RT_TRUE, __VA_ARGS__);                    \
    }                                                                         \
}                                                                             \
while (0)

#elif defined USING_RTOS_CONTIKI_NG
#define LORA_RADIO_DEBUG_LOG(type, level, ...)                                \
do                                                                            \
{                                                                             \
    if (type)                                                                 \
    {                                                                         \
        LOG(1, level, "DBG", __VA_ARGS__);                                    \
        LOG_OUTPUT("\r\n");                                                   \
    }                                                                         \
}                                                                             \
while (0)
#endif



#else /* LORA_RADIO_DEBUG */

#define LORA_RADIO_DEBUG_LOG(type, level, ...)

#endif /* LORA_RADIO_DEBUG */

#endif /* __LORA_RADIO_DEBUG_H__ */
