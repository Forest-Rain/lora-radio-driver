/*!
 * \file      lora-radio-spi-sx127x.c
 *
 * \brief     sx127x spi driver implementation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */

#include "sx127x-board.h"


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
