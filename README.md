# LoRa-Radio-Driver软件包 简介
  LoRa-Radio-Driver软件包是基于RTOS( RT-Thread ) 实现的LoRa Tranceiver芯片的驱动文件包，可用于快速搭建基于LoRa等通信的应用产品。

- 主要特点:
   - 当前支持LoRa Transceiver（sx126x\sx127x ）
      - 支持调制方式
         - [x] LoRa
         - [ ] FSK
   - 可通过menuconfig灵活定义LoRa模块的对外接口，降低入门门槛
   - 提供常用实例代码，可用于射频性能测试、空口数据包监听、双向通信测试等
   - 可作为phy层对接到LoRaWAN协议栈
- 当前支持LoRa 模块\芯片
   - LoRa Transceiver (SPI)
      - SX126X (SX1262\ASR6500S\LLCC68\SX1268..)
        - [x] [LSD4RF-2R717N40 ( SX1268 )](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87)
        - [x] ASR6500S ( SX1262 )
      - SX127X (SX1272\SX1276\SX1278..)
        - [x] [LSD4RF-2F717N20 ( SX1278 )](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87)
        - [x] Ra-01 ( SX1278 )
   - LoRa SIP\SoC

更多详细信息请查看[LoRa-Radio-Driver/doc](https://github.com/Forest-Rain/lora-radio-driver/tree/master/doc)
