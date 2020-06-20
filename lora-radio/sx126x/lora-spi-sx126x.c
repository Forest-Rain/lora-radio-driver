/*!
 * \file      lora-radio-spi.c
 *
 * \brief     spi driver implementation for lora-radio
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_spi.h"

#include "sx126x.h"
#include "sx126x-board.h"

#ifndef GET_GPIO_PORT
#define GET_GPIO_PORT(pin) (GPIO_TypeDef *)( GPIOA_BASE + (uint32_t) ( pin >> 4 ) * 0x0400UL ) // + for get GPIO,eg GPIOA
#endif
#ifndef GET_GPIO_PIN
#define GET_GPIO_PIN(pin) (rt_uint16_t)( 1 << ( pin & 0x0F ) ) // for get GPIO_PIN, eg: GPIO_PIN_6
#endif

#ifdef RT_USING_ULOG
#define DRV_DEBUG
#define LOG_TAG             "lora.radio.spi"
#include <drv_log.h>
#include <ulog.h> 
#else
#define LOG_D(...)
#define LOG_I(...)
#endif

extern void RadioOnDioIrq( void* context );
/*!
 * Debug GPIO pins objects
 */
#if defined( USE_RADIO_DEBUG )
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

struct rt_spi_device *lora_radio_spi_init(const char *bus_name,const char *lora_device_name, rt_uint8_t param)
{
    rt_err_t res;
    struct rt_spi_device *lora_radio_spi_device;
    
    RT_ASSERT(bus_name);

    {
        //res = rt_hw_spi_device_attach( bus_name, LORA_RADIO_DEVICE_NAME, GPIOA, GPIO_PIN_15);
        res = rt_hw_spi_device_attach( bus_name, lora_device_name, GET_GPIO_PORT(LORA_RADIO_NSS_PIN), GET_GPIO_PIN(LORA_RADIO_NSS_PIN));
        
        if (res != RT_EOK)
        {
            LOG_D("rt_spi_bus_attach_device failed!\r\n");
            return RT_NULL;
        }
        
        lora_radio_spi_device = (struct rt_spi_device *)rt_device_find(lora_device_name);
        if (!lora_radio_spi_device)
        {
            LOG_D("spi sample run failed! cant't find %s device!\n", lora_device_name);
            return RT_NULL;
        }
    }
    
    LOG_D("find %s device!\n", lora_device_name);
    
    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0. */
        cfg.max_hz = 8 * 1000000;             /* max 10M */
        
        res = rt_spi_configure(lora_radio_spi_device, &cfg);
        
        if (res != RT_EOK)
        {
            LOG_D("rt_spi_configure failed!\r\n");
        }
        res = rt_spi_take_bus(lora_radio_spi_device);
        if (res != RT_EOK)
        {
            LOG_D("rt_spi_take_bus failed!\r\n");
        }
        
        res = rt_spi_release_bus(lora_radio_spi_device);
        
        if(res != RT_EOK)
        {
            LOG_D("rt_spi_release_bus failed!\r\n");
        }
    }

    return lora_radio_spi_device;
} 

/**
 * This function releases memory
 *
 * @param dev the pointer of device driver structure
 */
void lora_radio_spi_deinit(struct rt_spi_device *dev)
{
    RT_ASSERT(dev);
    rt_spi_release_bus(dev);
}

#ifndef RT_USING_SPI
//uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
uint16_t SpiInOut( SPI_TypeDef *spi,uint16_t outData )
{   
    uint32_t timeout = 32000;//32M,32ms
	while((spi->SR&SPI_SR_TXE) != SPI_SR_TXE && timeout--){};//µÈ´ý·¢ËÍ¼Ä´æÆ÷¿Õ
    spi->DR = outData;
    timeout = 32000;
    while((spi->SR&SPI_SR_RXNE) != SPI_SR_RXNE && timeout--){};//µÈ´ý½ÓÊÕ¼Ä´æÆ÷·Ç¿Õ
    return spi->DR;
}
#endif

void SX126xWakeup( void )
{
#ifdef RT_USING_SPI
    uint8_t msg[2] = { RADIO_GET_STATUS, 0x00 };
    
    rt_spi_transfer(SX126x.spi,msg,RT_NULL,2);
    
    //rt_spi_send_then_send(SX126x.spi,&msg1,1,&msg2,1);
 
    // Wait for chip to be ready.
    SX126xWaitOnBusy( );
#else
    /////CRITICAL_SECTION_BEGIN( );

    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_LOW);
    SpiInOut( SPI3, RADIO_GET_STATUS );
    SpiInOut( SPI3, 0x00 );
    rt_pin_write(LORA_RADIO_NSS_PIN, PIN_HIGH);
    
//////    ////GpioWrite( &SX126x.Spi.Nss, 0 ); 

//////    SpiInOut( &SX126x.Spi, RADIO_GET_STATUS );
//////    SpiInOut( &SX126x.Spi, 0x00 );

//////    ////GpioWrite( &SX126x.Spi.Nss, 1 );
   
    
    // Wait for chip to be ready.
    SX126xWaitOnBusy( );

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
////    
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
////    
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
