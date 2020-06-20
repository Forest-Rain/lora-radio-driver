/*!
 * \file      lora-radio-spi.c
 *
 * \brief     spi driver implementation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_spi.h"
#include "sx127x-board.h"

#ifdef RT_USING_ULOG
#define DRV_DEBUG
#define LOG_TAG             "lora.radio.spi"
#include <drv_log.h>
#include <ulog.h> 
#else
#define LOG_D(...)
#define LOG_I(...)
#endif

#ifndef GET_GPIO_PORT
#define GET_GPIO_PORT(pin) (GPIO_TypeDef *)( GPIOA_BASE + (uint32_t) ( pin >> 4 ) * 0x0400UL ) // + for get GPIO,eg GPIOA
#endif
#ifndef GET_GPIO_PIN
#define GET_GPIO_PIN(pin) (rt_uint16_t)( 1 << ( pin & 0x0F ) ) // for get GPIO_PIN, eg: GPIO_PIN_6
#endif

extern void RadioOnDioIrq( void* context );

/*!
 * Debug GPIO pins objects
 */
#if defined( USE_RADIO_DEBUG )
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

struct rt_spi_device *lora_radio_spi_init(const char *bus_name, const char *lora_device_name, rt_uint8_t param)
{
    rt_err_t res;
    struct rt_spi_device *lora_radio_spi_device;
    
    RT_ASSERT(bus_name);
    
    {
        //res = rt_hw_spi_device_attach( bus_name, lora_device_name, GPIOA, GPIO_PIN_15);
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

void SX127xWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{   
#ifdef RT_USING_SPI
    struct rt_spi_message msg1, msg2;
    uint8_t    data = (addr | 0x80);
    msg1.send_buf   = &data;
    msg1.recv_buf   = RT_NULL;
    msg1.length     = 1;
    msg1.cs_take    = 1;
    msg1.cs_release = 0;
    msg1.next       = &msg2;

    msg2.send_buf   = buffer;
    msg2.recv_buf   = RT_NULL;
    msg2.length     = size;
    msg2.cs_take    = 0;
    msg2.cs_release = 1;
    msg2.next       = RT_NULL;
    rt_spi_transfer_message(SX127x.spi, &msg1);
#else
//    uint8_t i;

//    //NSS = 0;
//    GpioWrite( &SX127x.Spi.Nss, 0 );

//    SpiInOut( &SX127x.Spi, addr | 0x80 );
//    for( i = 0; i < size; i++ )
//    {
//        SpiInOut( &SX127x.Spi, buffer[i] );
//    }

//    //NSS = 1;
//    GpioWrite( &SX127x.Spi.Nss, 1 );
#endif
}

void SX127xReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
#ifdef RT_USING_SPI
    struct rt_spi_message msg1, msg2;
    uint8_t    data = (addr & 0x7F);
    msg1.send_buf   = &data;
    msg1.recv_buf   = RT_NULL;
    msg1.length     = 1;
    msg1.cs_take    = 1;
    msg1.cs_release = 0;
    msg1.next       = &msg2;

    msg2.send_buf   = RT_NULL;
    msg2.recv_buf   = buffer;
    msg2.length     = size;
    msg2.cs_take    = 0;
    msg2.cs_release = 1;
    msg2.next       = RT_NULL;
    rt_spi_transfer_message(SX127x.spi, &msg1);    
#else
//    uint8_t i;

//    //NSS = 0;
//    GpioWrite( &SX127x.Spi.Nss, 0 );

//    SpiInOut( &SX127x.Spi, addr & 0x7F );

//    for( i = 0; i < size; i++ )
//    {
//        buffer[i] = SpiInOut( &SX127x.Spi, 0 );
//    }

//    //NSS = 1;
//    GpioWrite( &SX127x.Spi.Nss, 1 );
#endif
}
