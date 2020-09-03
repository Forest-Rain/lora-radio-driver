/*!
 * \file      lora-spi-SX126x.c
 *
 * \brief     spi driver implementation for SX126X
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */
 
#include "lora-radio-rtos-config.h"

#include "sx126x.h"
#include "sx126x-board.h"

#define LOG_TAG "LoRa.SX126X.SPI"
#define LOG_LEVEL  LOG_LVL_DBG 
#include "lora-radio-debug.h"


void SX126xWakeup( void )
{
#ifdef RT_USING_SPI
    uint8_t msg[2] = { RADIO_GET_STATUS, 0x00 };
    
    rt_spi_transfer(SX126x.spi,msg,RT_NULL,2);
    
    //rt_spi_send_then_send(SX126x.spi,&msg1,1,&msg2,1);
 
    // Wait for chip to be ready.
    SX126xWaitOnBusy( );
#else
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);
    SpiInOut( SPI3, RADIO_GET_STATUS );
    SpiInOut( SPI3, 0x00 );
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);

    // Wait for chip to be ready.
    SX126xWaitOnBusy( );

    /////CRITICAL_SECTION_BEGIN( );
    //////    GpioWrite( &SX126x.Spi.Nss, 0 ); 

    //////    SpiInOut( &SX126x.Spi, RADIO_GET_STATUS );
    //////    SpiInOut( &SX126x.Spi, 0x00 );

    //////    GpioWrite( &SX126x.Spi.Nss, 1 );
    /////CRITICAL_SECTION_END( );
#endif
    
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{    
#ifdef RT_USING_SPI
    SX126xCheckDeviceReady( );

    rt_spi_send_then_send(SX126x.spi,&command,1,buffer,size);
    
    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
#else
    SX126xCheckDeviceReady( );

    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);
    SpiInOut( SPI3, ( uint8_t )command );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( SPI3, buffer[i] );
    }
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
   
////    GpioWrite( &SX126x.Spi.Nss, 0 );

////    SpiInOut( &SX126x.Spi, ( uint8_t )command );

////    for( uint16_t i = 0; i < size; i++ )
////    {
////        SpiInOut( &SX126x.Spi, buffer[i] );
////    }

////    GpioWrite( &SX126x.Spi.Nss, 1 );

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }    
#endif    
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
#ifdef RT_USING_SPI
    uint8_t status = 0;
    uint8_t buffer_temp[16] = {0}; // command size is 2 size
    
    SX126xCheckDeviceReady( );
    
    rt_spi_send_then_recv(SX126x.spi,&command,1,buffer_temp,size + 1);
    
    status = buffer_temp[0];
    
    rt_memcpy(buffer,buffer_temp+1,size);
    
    SX126xWaitOnBusy( );
    
    return status;
#else
    uint8_t status = 0;

    SX126xCheckDeviceReady( );

    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);
    SpiInOut( SPI3, ( uint8_t )command );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( SPI3, 0 );
    }
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
    
////    GpioWrite( &SX126x.Spi.Nss, 0 );

////    SpiInOut( &SX126x.Spi, ( uint8_t )command );
////    status = SpiInOut( &SX126x.Spi, 0x00 );
////    for( uint16_t i = 0; i < size; i++ )
////    {
////        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
////    }

////    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );  
#endif    
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
#ifdef RT_USING_SPI
    uint8_t msg[3] = {0};
    
    msg[0] = RADIO_WRITE_REGISTER;
    msg[1] = ( address & 0xFF00 ) >> 8;
    msg[2] = address & 0x00FF;
    
    SX126xCheckDeviceReady( );
    
    rt_spi_send_then_send(SX126x.spi,msg,3,buffer,size);

    SX126xWaitOnBusy( );
#else
    SX126xCheckDeviceReady( );

    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);

    SpiInOut( SPI3, RADIO_WRITE_REGISTER );
    SpiInOut( SPI3, ( address & 0xFF00 ) >> 8 );
    SpiInOut( SPI3, address & 0x00FF );
    
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( SPI3, buffer[i] );
    }
    
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
    
////    GpioWrite( &SX126x.Spi.Nss, 0 ); 
////    SpiInOut( &SX126x.Spi, RADIO_WRITE_REGISTER );
////    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
////    SpiInOut( &SX126x.Spi, address & 0x00FF );
////    
////    for( uint16_t i = 0; i < size; i++ )
////    {
////        SpiInOut( &SX126x.Spi, buffer[i] );
////    }
////    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
#endif    
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{ 
#ifdef RT_USING_SPI
    uint8_t msg[4] = {0};
    
    msg[0] = RADIO_READ_REGISTER;
    msg[1] = ( address & 0xFF00 ) >> 8;
    msg[2] = address & 0x00FF;
    msg[3] = 0;
    
    SX126xCheckDeviceReady( );

    rt_spi_send_then_recv(SX126x.spi,msg,4,buffer,size);

    SX126xWaitOnBusy( );
#else
    SX126xCheckDeviceReady( );

    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);

    SpiInOut( SPI3, RADIO_READ_REGISTER );
    SpiInOut( SPI3, ( address & 0xFF00 ) >> 8 );
    SpiInOut( SPI3, address & 0x00FF );
    SpiInOut( SPI3, 0 );
    
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( SPI3, 0 );
    }
    
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
    
////    GpioWrite( &SX126x.Spi.Nss, 0 );

////    SpiInOut( &SX126x.Spi, RADIO_READ_REGISTER );
////    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
////    SpiInOut( &SX126x.Spi, address & 0x00FF );
////    SpiInOut( &SX126x.Spi, 0 );
////    for( uint16_t i = 0; i < size; i++ )
////    {
////        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
////    }
////    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
#endif
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
#ifdef RT_USING_SPI

    uint8_t msg[2] = {0};
    
    msg[0] = RADIO_WRITE_BUFFER;
    msg[1] = offset;
    
    SX126xCheckDeviceReady( );
    
    rt_spi_send_then_send(SX126x.spi,msg,2,buffer,size);
    
    SX126xWaitOnBusy( );  

#else    

    SX126xCheckDeviceReady( );

    
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);

    SpiInOut( SPI3, RADIO_WRITE_BUFFER );
    SpiInOut( SPI3, offset );
    
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( SPI3, buffer[i] );
    }
    
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
    
////    GpioWrite( &SX126x.Spi.Nss, 0 );

////    SpiInOut( &SX126x.Spi, RADIO_WRITE_BUFFER );
////    SpiInOut( &SX126x.Spi, offset );
////    for( uint16_t i = 0; i < size; i++ )
////    {
////        SpiInOut( &SX126x.Spi, buffer[i] );
////    }
////    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

#endif
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
#ifdef RT_USING_SPI
    uint8_t msg[3] = {0};
    
    msg[0] = RADIO_READ_BUFFER;
    msg[1] = offset;
    msg[2] = 0;
    
    SX126xCheckDeviceReady( );

    rt_spi_send_then_recv(SX126x.spi,msg,3,buffer,size);

    SX126xWaitOnBusy( );
#else    
    SX126xCheckDeviceReady( );

    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);

    SpiInOut( SPI3, RADIO_READ_BUFFER );
    SpiInOut( SPI3, offset );
    SpiInOut( SPI3, 0 );
    
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( SPI3, 0 );
    }
    
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
    
////    GpioWrite( &SX126x.Spi.Nss, 0 );

////    SpiInOut( &SX126x.Spi, RADIO_READ_BUFFER );
////    SpiInOut( &SX126x.Spi, offset );
////    SpiInOut( &SX126x.Spi, 0 );
////    for( uint16_t i = 0; i < size; i++ )
////    {
////        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
////    }
////    GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
#endif
    
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}
