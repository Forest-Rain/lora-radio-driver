/*!
 * \file      sx126x-board.c
 *
 * \brief     Target board SX126x shield driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Forest-Rain
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "sx126x-board.h"
#include "drv_gpio.h" // for GET_PIN

#ifdef RT_USING_ULOG
#define DRV_DEBUG
#define LOG_TAG             "lora.radio.sx1268" // LSD4RF-2R717N40
#include <drv_log.h>
#include <ulog.h> 
#else
#define LOG_D(...)
#define LOG_I(...)
#endif

extern void RadioOnDioIrq( void* context );

void SX126xIoInit( void )
{   
    SX126x.Spi = (struct rt_spi_device *)rt_device_find(LORA_RADIO_DEVICE_NAME);
    if(SX126x.Spi == RT_NULL)
    {
        rt_kprintf("ERROR: NO SPI  DEVICE NAMED %s attached\n",LORA_RADIO_DEVICE_NAME);
        return;
    }     

    if(LORA_RADIO_BUSY_PIN != -1)
        rt_pin_mode(LORA_RADIO_BUSY_PIN, PIN_MODE_INPUT);

    if(LORA_RADIO_DIO1_PIN != -1)
        rt_pin_mode(LORA_RADIO_DIO1_PIN, PIN_MODE_INPUT_PULLDOWN);
    
    if(LORA_RADIO_DIO2_PIN != -1)
        rt_pin_mode(LORA_RADIO_DIO2_PIN, PIN_MODE_INPUT_PULLDOWN);

    if(LORA_RADIO_RFSW1_PIN != -1)
        rt_pin_mode(LORA_RADIO_RFSW1_PIN, PIN_MODE_OUTPUT);

    if(LORA_RADIO_RFSW2_PIN != -1)
        rt_pin_mode(LORA_RADIO_RFSW2_PIN, PIN_MODE_OUTPUT);
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{   
    if(LORA_RADIO_DIO1_PIN != -1)
    {
        rt_pin_attach_irq(LORA_RADIO_DIO1_PIN, PIN_IRQ_MODE_RISING, RadioOnDioIrq,(void*)"callback args");
        rt_pin_irq_enable(LORA_RADIO_DIO1_PIN, PIN_IRQ_ENABLE);
    }
}

void SX126xIoDeInit( void )
{
}


void SX126xIoTcxoInit( void )
{
    // Initialize TCXO control
    CalibrationParams_t calibParam;
    
    // +clear OSC_START_ERR for reboot or cold-start from sleep
    SX126xClearDeviceErrors();
    
    // TCXO_CTRL_1_7V -> TCXO_CTRL_2_7V 64*15.0625US
    SX126xSetDio3AsTcxoCtrl( TCXO_CTRL_2_7V, 320);//SX126xGetBoardTcxoWakeupTime( ) << 6 ); // convert from ms to SX126x time base
    
    calibParam.Value = 0x7F;
    SX126xCalibrate( calibParam );
}

uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xReset( void )
{    
    DelayMs( 10 );
    rt_pin_mode(LORA_RADIO_RESET_PIN, PIN_MODE_OUTPUT); 
    rt_pin_write(LORA_RADIO_RESET_PIN, PIN_LOW);
    DelayMs( 20 );
     // internal pull-up
    rt_pin_mode(LORA_RADIO_RESET_PIN, PIN_MODE_INPUT); 
    DelayMs( 10 ); 
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
    if(LORA_RADIO_RFSW1_PIN != -1)
        rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_LOW);

    if(LORA_RADIO_RFSW2_PIN != -1)
        rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_LOW);
}

void SX126xSetAntSw( RadioOperatingModes_t mode )
{
    if((LORA_RADIO_RFSW1_PIN == -1) || (LORA_RADIO_RFSW2_PIN == -1))
        return;
    
    if( mode == MODE_TX )
    // Transmit
    {
        rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_HIGH);
        rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_LOW);
    }
    else 
    {
        rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_LOW);
        rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_HIGH);
    }

}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
