/*!
 * \file      lora-spi-board.c
 *
 * \brief     spi peripheral initlize,it depend on mcu platform.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */
#include "sx127x-board.h"

#define LOG_TAG "LoRa.STM32.SPI"
#define LOG_LEVEL  LOG_LVL_DBG 
#include "lora-radio-debug.h"

// for get GPIO fort,eg GPIOA
#ifndef GET_GPIO_PORT
#define GET_GPIO_PORT(pin) (GPIO_TypeDef *)( GPIOA_BASE + (uint32_t) ( pin >> 4 ) * 0x0400UL ) 
#endif
#ifndef GET_GPIO_PIN
#define GET_GPIO_PIN(pin) (rt_uint16_t)( 1 << ( pin & 0x0F ) ) // for get GPIO_PIN, eg: GPIO_PIN_6
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
