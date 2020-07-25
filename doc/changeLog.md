### 2020/07/06

1. 删除了代码中多余的注释代码

2. 删除了代码中**USING_LORA_RADIO_DRIVER_RTOS_SUPPORT**,因为即使不开启这个宏定义，当前版本的代码也无法在非rt-thread full的环境中运行，所以这个宏定义有些多余

3. 删除spi初始化，因为spi配置部分和具体MCU型号有耦合，而lora radio的运行本身和MCU型号并没有关联，所以不应只能跑在某一特定MCU上

4. 修改了KConfig文件，因为原先的文件中有很多的不合理(比如多个芯片型号应该只能多选一，而不是允许多选)，代码也相对杂乱

5. 因为3的修改，需要用户在使用前添加一段spi接口挂载和配置的代码，其中不同MCU平台，其挂载的方法可能会有不同，如下(STM32p平台):

   ```
   int lora_spi_prepare(void)
   {
       rt_err_t res;
       struct rt_spi_device *lora_radio_spi_device;
   
       #define NSS_PIN GET_PIN(B, 6)
       
       res = rt_hw_spi_device_attach( "spi1", "spi1.1", GPIOB, GPIO_PIN_6);
       if (res != RT_EOK)
       {
           rt_kprintf("rt_spi_bus_attach_device failed!\r\n");
           return RT_NULL;
       }
       
       rt_pin_mode(NSS_PIN,PIN_MODE_OUTPUT);
       rt_pin_write(NSS_PIN, PIN_HIGH);
       
       lora_radio_spi_device = (struct rt_spi_device *)rt_device_find("spi1.1");
       if (!lora_radio_spi_device)
       {
           rt_kprintf("spi sample run failed! cant't find %s device!\n", "spi1.1");
           return RT_NULL;
       }
       
       /* config spi */
       struct rt_spi_configuration cfg;
       cfg.data_width = 8;
       cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0. */
       cfg.max_hz = 8 * 1000000;             /* max 8M */
       
       res = rt_spi_configure(lora_radio_spi_device, &cfg);
       if (res != RT_EOK)
       {
           rt_kprintf("rt_spi_configure failed!\r\n");
       }
       
       return 0;
   }
   INIT_DEVICE_EXPORT(lora_spi_prepare);
   ```