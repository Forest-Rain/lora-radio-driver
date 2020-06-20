# LoRa-Radio-Driver软件包

# 1 简介
LoRa-Radio-Driver软件包是基于RTOS( RT-Thread ) 实现的LoRa Tranceiver芯片的驱动文件，可用于快速搭建基于LoRa等通信的应用产品。<br />LoRa-Radio-Driver软件包在LoRaWAN开源协议栈[LoRaMAC-Node中的radio]()基础上，进一步封装实现。
> LoRaMac\Radio
> [https://github.com/Lora-net/LoRaMac-node/tree/master/src/radio](https://github.com/Lora-net/LoRaMac-node/tree/master/src/radio)

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
      - SX126X
        - [x] LSD4WN-2R717N40 ( SX1268 )
      - SX127X
        - [x] LSD4WN-2F717N20 ( SX1278 )
        - [x] Ra-01 ( SX1278 )


## 1.1 LoRa Radio Driver 软件包组织结构
 <br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/253586/1592539008368-2a9e7389-2b06-4d17-a5d2-0ad9865c830c.png#align=left&display=inline&height=650&margin=%5Bobject%20Object%5D&name=image.png&originHeight=650&originWidth=1182&size=70704&status=done&style=none&width=1182)

- lora-radio
   - sx126x\127x驱动包
      - lora-radio-sx126x.c  \ lora-radio-sx127x.c 
         - 对外提供了上层访问接口
      - lora-radio-spi.c
         - spi接口实现
            - rt_device与裸机方式
      - sx126x.c \ sx127x.c
         - lora芯片底层驱动
- samples
   - lora radio driver示例文件
- port
   - 主要包含支持的lora模块
      - 文件夹中的xxxx-borad.c包含了主要的LoRa模块硬件接口配置：
         - LoRa模块功率输出方式（PA\RFO...）
         - LoRa模块的RF高频开关控制方式（TXE、RXE、无..）
         - LoRa模块的DIO口（DIO0、DIO1、DIO2....）
         - LoRa模块的工作频率限制等

# 2 LoRa Radio Driver软件包使用说明

## 2.1 依赖

- multi-rtimer软件包
   - 用于提供定时器超时服务
   - 如果用于时间精度要求高(ms级别)的场景，注意开启LSE，并设置RTC时钟源为外部32768晶振
- 可选
   - ulog组件
      - lora-raido-driver内部使用ulog接口，若使能ulog，则可看到更多调试输出信息
> RT-Thread Components --->
>    Utiliess --->
>        [*] Enable ulog
> [*] Enable ISR log.

<br />

## 2.2 获取软件包

<br />使用 lora-radio-timer 软件包，需要在 RT-Thread 的包管理中选中它，具体路径如下：<br />

```
RT-Thread online packages --->
    peripheral libraries and drivers --->
        [*] lora_radio_driver: lora chipset(sx126x\sx127x.)driver. --->
                (lora-radio0)LoRa Radio Device Name                
                [] Select LoRa Chip \ LoRa Module --->
                []    LoRa Transceiver [SX126X]
                []    LoRa Transceiver [SX127X]
                Select LoRa Radio Driver Example ---> 	
                Version (latest)  --->
```


1. 选择lora模块，根据实际使用的MCU硬件平台与lora模块，配置SPI与GPIO引脚等功能选项
   - 设定SPI口
   - 设定LoRa模块的GPIO口（比如 RESET、NSS、BUSY、DIO1、TXE、RXE...）
2. 根据实际情况，可选择测试示例


## 2.3 新增LoRa模块
在 lora-radio-driver\ports\lora-module文件下，参考已有模板，增加新的lora模块驱动文件xxxx-board.c，<br />

## 2.4 使用示例

## 2.4.1 Finish测试命令
若使能[* ] LoRa Radio Test，则可以通过finish命令进行LoRa测试<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/253586/1592550763879-ac947241-b072-4db8-89bf-ced6ab168af9.png#align=left&display=inline&height=163&margin=%5Bobject%20Object%5D&name=image.png&originHeight=163&originWidth=858&size=21400&status=done&style=none&width=858)

| 序号 | finish命令 | 说明 |
| --- | --- | --- |
| 1 | lora probe | 测试lora设备(SPI)是否访问正常 |
| 2 | lora cw <para1> <para2> | <para1>:频点，单位Hz<br /><para2>:功率，单位dBm<br /> |
| 3 | lora ping <para1> <para2> | <para1> : 主机\从机<br />- -m 主机<br />- -s 从机<br />
<para2>: 发送数据包个数 |
| 4 | lora rx  | 接收数据包，同时以16进制格式与ASCII码显示数据内容 |
 <br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/253586/1592551287552-a4268ba4-06e7-4a0c-bf8a-cc5742cb0a78.png#align=left&display=inline&height=890&margin=%5Bobject%20Object%5D&name=image.png&originHeight=890&originWidth=1306&size=223346&status=done&style=none&width=1306)<br />lora ping 双向通信测试示例<br />

<br />![image.png](https://cdn.nlark.com/yuque/0/2020/png/253586/1592551532403-4f10f828-3eb3-406e-af6d-45ddfaf6310e.png#align=left&display=inline&height=816&margin=%5Bobject%20Object%5D&name=image.png&originHeight=816&originWidth=1704&size=213381&status=done&style=none&width=1704)<br />单向通信测试示例<br />
