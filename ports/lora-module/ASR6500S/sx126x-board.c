/*
 * @Author: your name
 * @Date: 2020-07-04 16:27:36
 * @LastEditTime: 2020-07-06 02:37:31
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \stm32l476-st-nucleo\packages\lora_radio_driver-latest\ports\lora-module\ASR6500S\sx126x-board.c
 */ 
/*
 * author    Rick Zhang
 * date      2020.07.20
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "sx126x-board.h"

#define DRV_DEBUG
#define LOG_TAG             "lora.radio.sx1262" // ASR6500S
#include <drv_log.h>

extern void RadioOnDioIrq( void* context );

void SX126xIoInit( void )
{
    SX126x.Spi = (struct rt_spi_device *)rt_device_find(LORA_RADIO_DEVICE_NAME);
    if(SX126x.Spi == RT_NULL)
    {
        rt_kprintf("ERROR: NO SPI  DEVICE NAMED %s attached\n",LORA_RADIO_DEVICE_NAME);
        return;
    }    
    rt_pin_mode(LORA_RADIO_BUSY_PIN, PIN_MODE_INPUT);
    rt_pin_mode(LORA_RADIO_DIO1_PIN, PIN_MODE_INPUT);
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    rt_pin_mode(LORA_RADIO_DIO1_PIN, PIN_MODE_INPUT);
    rt_pin_attach_irq(LORA_RADIO_DIO1_PIN, PIN_IRQ_MODE_RISING, RadioOnDioIrq,(void*)"callback args");
    rt_pin_irq_enable(LORA_RADIO_DIO1_PIN, PIN_IRQ_ENABLE);
}

void SX126xIoDeInit( void )
{
}

void SX126xIoTcxoInit( void )
{
    CalibrationParams_t calibParam;
    SX126xSetDio3AsTcxoCtrl( TCXO_CTRL_1_8V, SX126xGetBoardTcxoWakeupTime( ) << 6 ); // convert from ms to SX126x time base
    calibParam.Value = 0x7F;
    SX126xCalibrate( calibParam );
}

uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xReset( void )
{
    DelayMs( 20 );
    rt_pin_mode(LORA_RADIO_RESET_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LORA_RADIO_RESET_PIN, PIN_LOW);
    DelayMs( 40 );
    rt_pin_mode(LORA_RADIO_RESET_PIN, PIN_MODE_INPUT);
    DelayMs( 20 );
}

void SX126xWaitOnBusy( void )
{
    while( rt_pin_read( LORA_RADIO_BUSY_PIN ) == PIN_HIGH );
}

void SX126xAntSwOn( void )
{
}

void SX126xAntSwOff( void )
{
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    return true;
}
void SX126xSetAntSw( RadioOperatingModes_t mode )
{
}
