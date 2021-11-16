#ifndef __LORA_RADIO_TIMER_H__
#define __LORA_RADIO_TIMER_H__

#include "lora-radio-rtos-config.h"
#include <stdint.h>

#ifdef PKG_USING_MULTI_RTIMER

#include "multi_rtimer.h"
#include "hw_rtc_stm32.h"

#define LORA_RADIO_TIMESTAMP_TO_MS(x) (x)

#else

#ifdef LORA_RADIO_DRIVER_USING_RTOS_RT_THREAD

/** @addtogroup LORA_RADIO_TIMER
  * @{
  */

#define TimerInit            rtick_timer_init
#define TimerStart           rtick_timer_start
#define TimerStop            rtick_timer_stop   
#define TimerReset           rtick_timer_reset
#define TimerSetValue        rtick_timer_set_value
#define TimerSetTimestamp    rtick_timer_set_timestamp
#define TimerGetCurrentTime  rtick_timer_get_current_time
#define TimerGetElapsedTime  rtick_timer_get_elapsed_time
#define TimerEvent_t         rtick_timer_event_t

#define LORA_RADIO_TIMESTAMP_TO_MS(x) ((x) * 1000 / RT_TICK_PER_SECOND)
/*!
 * \brief Timer object description
 */
typedef struct TimerEvent_s
{
    struct rt_timer timer; 
}rtick_timer_event_t;

/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#define TIMERTIME_T_MAX                             ( ( uint32_t )~0 )

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void rtick_timer_init( rtick_timer_event_t *obj, void ( *callback )( void ) );

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void rtick_timer_start( rtick_timer_event_t *obj );

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void rtick_timer_stop( rtick_timer_event_t *obj );

/*!
 * \brief Resets the timer object
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void rtick_timer_reset( rtick_timer_event_t *obj );

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
void rtick_timer_set_value( rtick_timer_event_t *obj, uint32_t value );

void rtick_timer_set_timestamp( rtick_timer_event_t *obj, uint32_t value );

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
TimerTime_t rtick_timer_get_current_time( void );

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \remark TimerGetElapsedTime will return 0 for argument 0.
 *
 * \param [IN] past         fix moment in Time
 * \retval time             returns elapsed time
 */
TimerTime_t rtick_timer_get_elapsed_time( TimerTime_t past );

/**
  * @}
  */

#endif

#endif

#endif

#endif
