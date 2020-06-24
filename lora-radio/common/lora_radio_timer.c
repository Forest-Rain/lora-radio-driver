/*
 * @file 
 * @Date 2020-06-17 17:38:34
 * @LastEditTime: 2020-06-24 15:48:17
 * @Description 
 *      lora_radio/lorawan timer
 */ 

#include <rtconfig.h>
#ifndef PKG_USING_MULTI_RTIMER

#include <rtthread.h>
#include <stdint.h>
#include "lora_radio_timer.h"

void rtick_timer_init( rtick_timer_event_t *obj, void ( *callback )( void ) )
{
    int count = 0;
    
    char name[RT_NAME_MAX];
    rt_snprintf(name,8,"rtk_%d",count);

    rt_timer_init(&(obj->timer),name,(void (*)(void*))callback,RT_NULL,1000,RT_IPC_FLAG_FIFO);
}

void rtick_timer_start( rtick_timer_event_t *obj )
{
    rt_timer_start(&(obj->timer));
}

void rtick_timer_stop( rtick_timer_event_t *obj )
{
    rt_timer_stop(&(obj->timer));
}

void rtick_timer_reset( rtick_timer_event_t *obj )
{
    rtick_timer_stop(obj);
    rtick_timer_start(obj);
}

void rtick_timer_set_value( rtick_timer_event_t *obj, uint32_t value )
{
    rt_timer_control(&(obj->timer),RT_TIMER_CTRL_SET_TIME,&value);
}

TimerTime_t rtick_timer_get_current_time( void )
{
    uint32_t now = rt_tick_get();
    return  now;
}

TimerTime_t rtick_timer_get_elapsed_time( TimerTime_t past )
{
    return rt_tick_get() - past;
}

#endif

