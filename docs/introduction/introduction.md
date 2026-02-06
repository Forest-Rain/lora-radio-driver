# 1 简介

LoRa-Radio-Driver软件包是基于RTOS( RT-Thread ) 实现的LoRa Tranceiver芯片(SX126x、SX127x等)的驱动文件，该驱动文件通过SPI访问LoRa Tranceiver芯片，可用于快速搭建基于LoRa等通信的应用产品。
LoRa-Radio-Driver软件包在LoRaWAN开源协议栈[LoRaMAC-Node中的radio](https://github.com/Forest-Rain/lora-radio-driver/blob/master/doc)基础上，进一步封装实现。

> LoRaMac\Radio
> [https://github.com/Lora-net/LoRaMac-node/tree/master/src/radio](https://github.com/Lora-net/LoRaMac-node/tree/master/src/radio)

- 主要特点:
  - 当前支持LoRa Transceiver（sx126x\sx127x ）
    - 支持调制方式
      - [x] LoRa
      - [ ] FSK
  - 可通过EVN工具menuconfig直接定义LoRa模块的对外接口，降低入门门槛
    - 支持使用引脚号来定义GPIO
    - 支持使用引脚名来定义GPIO
  - 提供常用实例代码，可用于射频性能测试、空口数据包监听、双向通信测试等
  - 可作为phy层对接到LoRaWAN End-Device协议栈
  - 当前测试的LoRa 模块\芯片
    - LoRa Transceiver (SPI)
      - SX126X (SX1262\ASR6500S\LLCC68\SX1268..)
        - SX1268
          - [x] [LSD4RF-2R717N40](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87)
        - SX1262
        - [x] ASR6500S
        - LLCC68
        - LR1110
      - SX127X (SX1272\SX1276\SX1278..)
        - SX1278
          - [x] [LSD4RF-2F717N20](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87)
          - [x] [Ra-01](http://wiki.ai-thinker.com/lora/man)
      - [ ] SX1276
    - LoRa SIP\SoC
  - 当前测试的MCU平台
    - LoRa Radio Driver当前功能主要在STM32L平台测试通过，未来计划将适配更多的MCU平台（华大MCU、nRF、BK）
      - [x] STM32L0系列
      - [x] STM32L4系列
  - 当前支持的RTOS
    - [x] RT-Thread
    - [ ] RT-Thread Nano

# 2 LoRa Radio Driver 软件包组织结构

![image.png](../image/01_lora-radio-driver_pkgs_struction.png)

- lora-radio
  - sx126x
    - lora-radio-sx126x.c
      - 对外提供了上层访问接口实现
    - lora-spi-sx126x.c
      - sx126x芯片的spi读写接口实现，独立于MCU平台
        - [x] rt_device
        - [ ] SPI裸机方式
    - sx126x.c
      - lora芯片sx126x底层驱动
  - sx127x
    - lora-radio-sx127x.c
      - 对外提供了上层访问接口
    - lora-spi-sx127x.c
      - sx127x芯片的spi读写接口实现，独立于MCU平台
        - [x] rt_device
        - [ ] SPI裸机方式
    - sx127x.c
      - lora芯片sx127x底层驱动
  - common
    - lora-radio-timer.c
      - 提供了lora-radio所需的定时服务接口，用于发送与接收超时等，基于RT-Thread内核rt_timer实现
        - 注意这种方式提供的定时最小颗粒度取决于系统tick RT_TICK_PER_SECOND
        - 注:如果使能了Multi-Rtimer软件包，则优先使用Multi-Rtimer提供定时\超时服务
  - include
    - lora-radio.h
      - 上层服务接口
    - lora-radio-debug.h
      - 根据需要使能输出lora-radio不同层级的调试信息
    - lora-radio-rtos-config.h
      - rtos适配层，选择，当前默认为RT-Thread
      - 未来支持RT-Thread-Nano、以及其他RTOS....
- samples
  - lora radio driver示例文件
    - lora-radio-test-shell
      - shell示例，主要实现了射频性能测试、空口数据包监听、双向通信测试等shell命令，便于日常测试
- port
  - 主要包含当前在不同MCU平台下支持的lora模块，lora-module文件夹中的xxxx-borad.c包含了与LoRa模块直接相关的主要硬件接口配置：
    - lora-module
      - stm32_adapter
        - lora-board-spi.c
          - STM32平台的SPI外设初始化等通用接口
        - LSD4RF-2F717N20 （SX1278 LoRa模块）
        - LSD4RF-2R717N40 （SX1268 LoRa模块）
        - Ra-01 （SX1278 LoRa模块）
          - xxxx-borad.c
            - LoRa模块功率输出方式（PA\RFO...）
            - LoRa模块的RF高频开关控制方式（TXE、RXE、无..）
            - LoRa模块的DIO口（DIO0、DIO1、DIO2....）
            - LoRa模块的工作频率限制等
      - xxx_adapter
        - 其他mcu平台下的硬件接口实现

# 3 问题和建议

如果有什么问题或者建议欢迎提交 [Issue](https://github.com/Forest-Rain/lora-radio-driver/issues) 进行讨论。
