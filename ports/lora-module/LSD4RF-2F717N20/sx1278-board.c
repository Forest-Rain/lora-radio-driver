/*!
 * \file      sx1278-board.c
 *
 * \brief     Target board SX127x driver implementation
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
#include "drv_gpio.h"

#include "lora-radio.h"
#include "sx127x-board.h"


/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * \brief DIO 0 IRQ callback
 */
void SX127xOnDio0IrqEvent( void *args );

/*!
 * \brief DIO 1 IRQ callback
 */
void SX127xOnDio1IrqEvent( void *args );

/*!
 * \brief DIO 2 IRQ callback
 */
void SX127xOnDio2IrqEvent( void *args );

/*!
 * \brief DIO 3 IRQ callback
 */
void SX127xOnDio3IrqEvent( void *args );

/*!
 * \brief DIO 4 IRQ callback
 */
void SX127xOnDio4IrqEvent( void *args );

/*!
 * \brief DIO 5 IRQ callback
 */
void SX127xOnDio5IrqEvent( void *args );

void SX127xIoInit( void )
{
    SX127x.Spi = (struct rt_spi_device *)rt_device_find(LORA_RADIO_DEVICE_NAME);
    if(SX127x.Spi == RT_NULL)
    {
        rt_kprintf("ERROR: NO SPI  DEVICE NAMED %s attached\n",LORA_RADIO_DEVICE_NAME);
        return;
    }

    if(LORA_RADIO_DIO0_PIN != -1)
        rt_pin_mode(LORA_RADIO_DIO0_PIN, PIN_MODE_INPUT_PULLDOWN);

    if(LORA_RADIO_DIO1_PIN != -1)
        rt_pin_mode(LORA_RADIO_DIO1_PIN, PIN_MODE_INPUT_PULLDOWN);

    if(LORA_RADIO_DIO2_PIN != -1)
        rt_pin_mode(LORA_RADIO_DIO2_PIN, PIN_MODE_INPUT_PULLDOWN);

}

void SX127xIoIrqInit( DioIrqHandler **irqHandlers )
{
    if(LORA_RADIO_DIO0_PIN != -1)
    {
        rt_pin_attach_irq(LORA_RADIO_DIO0_PIN, PIN_IRQ_MODE_RISING,SX127xOnDio0IrqEvent,(void*)"rf-dio0-int");
        rt_pin_irq_enable(LORA_RADIO_DIO0_PIN, PIN_IRQ_ENABLE);
    }
    if(LORA_RADIO_DIO1_PIN != -1)
    {
        rt_pin_attach_irq(LORA_RADIO_DIO1_PIN, PIN_IRQ_MODE_RISING,SX127xOnDio1IrqEvent,(void*)"rf-dio1-int");
        rt_pin_irq_enable(LORA_RADIO_DIO1_PIN, PIN_IRQ_ENABLE);    
    }

    if(LORA_RADIO_DIO2_PIN != -1)
    {
        rt_pin_attach_irq(LORA_RADIO_DIO2_PIN, PIN_IRQ_MODE_RISING,SX127xOnDio2IrqEvent,(void*)"rf-dio2-int");
        rt_pin_irq_enable(LORA_RADIO_DIO2_PIN, PIN_IRQ_ENABLE);
    }
}

void SX127xIoDeInit( void )
{
}

void SX127xReset( void )
{
    // Set RESET pin to 0
    rt_pin_mode(LORA_RADIO_RESET_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LORA_RADIO_RESET_PIN, PIN_LOW);
    
    // Wait 1 ms
    DelayMs( 1 );

    // Configure RESET as input
    rt_pin_mode(LORA_RADIO_RESET_PIN, PIN_MODE_INPUT);

    // Wait 6 ms
    DelayMs( 6 );
}

void SX127xSetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;

        if( status == false )
        {
            SX127xAntSwInit( );
        }
        else
        {
            SX127xAntSwDeInit( );
        }
    }
}

void SX127xAntSwInit( void )
{
    rt_pin_mode(LORA_RADIO_RFSW1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LORA_RADIO_RFSW2_PIN, PIN_MODE_OUTPUT);
    
    rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_LOW);
    rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_LOW);
}

void SX127xAntSwDeInit( void )
{    
    rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_LOW);
    rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_LOW);
}

void SX127xSetAntSw( uint8_t opMode )
{
    if((LORA_RADIO_RFSW1_PIN == -1) || (LORA_RADIO_RFSW2_PIN == -1))
        return;

    uint8_t paConfig =  SX127xRead( REG_PACONFIG );
    switch( opMode )
    {
    case RFLR_OPMODE_TRANSMITTER:
        if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
        {
            rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_HIGH);
            rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_LOW);
        }
        break;
    case RFLR_OPMODE_RECEIVER:
    case RFLR_OPMODE_RECEIVER_SINGLE:
    case RFLR_OPMODE_CAD:
    default:
        rt_pin_write(LORA_RADIO_RFSW1_PIN, PIN_LOW);
        rt_pin_write(LORA_RADIO_RFSW2_PIN, PIN_HIGH);
        break;
    }
}

uint8_t SX127xGetPaSelect( int8_t power )
{
    return RF_PACONFIG_PASELECT_PABOOST;
}

bool SX127xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported,todo depend on board
    return true; 
}


