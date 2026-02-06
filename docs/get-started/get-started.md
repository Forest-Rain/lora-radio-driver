# 1 系统依赖

- SPI外设——用户需根据实际MCU平台，自定义LoRa模块实际所需要使用的SPI外设
  - 选择SPI外设

```kconfig
  Hardware Drivers Config --->
     On-chip Peripheral Drivers --->
         [*] Enable SPI  --->
                 --- Enable SPI
                 [ ]   Enable SPI1
                 [ ]   Enable SPI2
                 [ ]   Enable SPI3
                 [ ]   Enable SPI4
                 [ ]   Enable SPI5
```

- 在bsp\目标板XX\board\Kconfig增加如下定义

```kconfig
  menuconfig BSP_USING_SPI
       bool "Enable SPI"
       select RT_USING_SPI

       if BSP_USING_SPI
         config BSP_USING_SPI1
            bool "Enable SPI1"
            default n
            if BSP_USING_SPI1
              config BSP_SPI1_RX_USING_DMA
                bool "Enable SPI1 RX DMA"
                default n
              config BSP_SPI1_TX_USING_DMA
                bool "Enable SPI1 TX DMA"
                default n
            endif
            
            # 根据实际需要，增加其他BSP_USING_SPI2、BSP_USING_SPI3...
        endif
```

- 定时服务——用于提供射频通信所需的定时\超时服务，目前支持以下两种方式，二选一
  - 内核 SOFT_TIMER
    - 若未选用Multi-Rtimer软件包，则默认采用内核的rt_timer来提供定时服务(lora-radio-timer.c)
    - 注意检测是否**开启RT-Thread内核的SOFT_TIMER**
  - M[ulti-Rtimer](https://github.com/Forest-Rain/multi-rtimer)软件包
    - 若使能multi-rtimer，lora-radio-driver优先使用multi-rtimer提供定时\超时服务。

> 注：如果应用在工业温度范围、时间精度要求高(us\ms级别)的场景，建议使用multi-rtimer，并设置RTC时钟源为外部32768晶振，否则可能会出现下行丢包的情况。

```kconfig
RT-Thread online packages --->
    peripheral libraries and drivers --->
        [*] multi_rtimer: a real-time and low power software timer module. --->
                Version (latest)  --->
                multi_rtimer options --->
                    [] multi_rtimer demo example
```

- 可选内核组件
  - ulog组件——用于打印日志信息
    - 使能ulog
      - ulog缓存大小设置≥ 128 Byte
      - lora-raido-driver内部可看到更多LoRa底层的调试信息
      - lora-radio-test-shell.c使用ulog接口，用于打印调试信息、原始16进制数据等
    - 如果没有使用ulog，默认使用rt_kprintf来实现信息输出功能

```kconfig
RT-Thread Components --->
   Utiliess --->
       [*] Enable ulog
           [*] Enable ISR log.
```

# 2 获取软件包

使用 lora-radio-driver 软件包，需要在 RT-Thread 的包管理中选中它，具体路径如下：

```kconfig
RT-Thread online packages --->
    peripheral libraries and drivers --->
        [*] lora_radio_driver: lora chipset(sx126x/sx127x.)driver. --->
            Select LoRa Radio Object Type (LoRa Radio Single-Instance)
                (lora-radio0)Setup LoRa Radio Device Name       
                (spi3)  Setup LoRa Radio Spi Name (Define BSP_USING_SPIx in [Target Platform]\Board\Kconfig)
                        Select LoRa Chip Type (LoRa Transceiver [SX126X])  --->
                        Select Supported LoRa Module [SX126X]  --->
                [ ]     Enable LoRa Radio Debug
                        Select LoRa Radio Driver Sample --->  
                    Version (latest)  --->
```

1. Select LoRa Chip/LoRa Module
   1. "Setup LoRa Radio Device Name"
      1. 设置LoRa Radio设备名称，缺省为"lora-radio0"
   2. "Setup LoRa Radio Spi Name"
      1. 设置LoRa Radio Spi名称
      1. 若在 [Target Platform]\Board\Kconfig提前设定好所使用的BSP_USING_SPIx，则会自动配置
   3. "Select LoRa Radio Single-Instance"
      1. 选择为单实例对象，当前只支持单个lora设备
   4. "Select LoRa Chip Type"
      1. 选择实际使用的LoRa芯片类型
         - 当前支持 SX126X、SX127x Transceiver
   5. "Select Supported LoRa Module"
      1. 选择lora模块，根据实际使用的MCU硬件平台与lora模块，配置关联的GPIO引脚等功能选项
         1. 设定LoRa模块的GPIO口（比如 RESET、NSS、BUSY、DIO1、TXE、RXE...）
            - " Select LoRa Chip GPIO by Pin Number  "
               - 支持使用引脚号来定义GPIO，比如 输入 10 代表 A10
            - "Select LoRa Chip GPIO by Pin Name"
               - 支持使用引脚名来定义GPIO，比如 输入 A10 代表引脚GPIOA的PIN10脚 (STM32)
2. Select LoRa Radio Driver Sample
   1. 根据实际情况，可选择测试示例

# 3 新增LoRa模块

在 lora-radio-driver\ports\lora-module文件下，参考已有模板，根据实际需要增加新的mcu平台适配文件、新的lora模块驱动文件xxxx-board.c

# 4 使用示例

## 4.1 硬件测试平台

当前所使用的硬件测试平台如下所示

| 序号 | 硬件平台 | MCU | LoRa模块 | 主要用户接口 |
| --- | --- | --- | --- | --- |
| 1 | LSD4RF-TEST2002  | STM32L476VG | [LSD4RF-2R717N40](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87)<br />[( SX1268 )](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87) | <br />- 用户接口定义<br />   - VCC  - 3.3V<br />   - GND<br />   - SCK    - PC10 (SPI3)<br />   - MISO  - PC11 (SPI3)<br />   - MOSI  - PC12 (SPI3)<br />   - NSS    - PA15<br />   - RESET - PA7<br />   - DIO0  - PB1<br />   - BUSY - PB2<br />   - RFSW1 - PB0<br />   - RFSW2 - PC5<br />- 射频开关TX trace<br />   - TX: RFSW1 = 1 , RFSW2 = 0<br />   - TX: RFSW1 = 0 , RFSW2 = 1<br /> |
| 2 | LSD4RF-TEST2002  | STM32L476VG | [LSD4RF-2F717N20](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87)<br />[( SX1278 )](http://bbs.lierda.com/forum.php?mod=viewthread&tid=87) | <br />- 用户接口定义<br />   - VCC   - 3.3V<br />   - GND<br />   - SCK    - PC10 (SPI3)<br />   - MISO  - PC11 (SPI3)<br />   - MOSI  - PC12 (SPI3)<br />   - NSS    - PB6<br />   - RESET - PA7<br />   - DIO0  - PB1<br />   - DIO1  - PC4<br />   - DIO2  - PB2<br />   - DIO3  - NC<br />   - DIO4  - NC<br />   - RFSW1 - PB0<br />   - RFSW2 - PC5<br />- 射频开关TX trace<br />   - TX: RFSW1 = 1 , RFSW2 = 0<br />   - TX: RFSW1 = 0 , RFSW2 = 1<br /> |
| 3 | Nucleo-L476RG | STM32L476RG | [Ra-01](http://wiki.ai-thinker.com/lora/man)<br />(RT-thread LoRa Adruino扩展板V1) | <br />- 用户接口定义<br />   - VCC    - 3.3V<br />   - GND<br />   - SCK    - PA5(SPI1)<br />   - MISO  - PA6(SPI1)<br />   - MOSI  - PA7(SPI1)<br />   - NSS    - PB6<br />   - RESET - PC7<br />   - DIO0  - PA9<br />   - DIO1  - PA8<br /> |

## 4.2 Shell测试命令

若使能 [* ] LoRa Radio Test Shell，则可以通过shell（finish）命令直接进行LoRa相关测试

```kconfig
[*] Enable LoRa Radio Test Shell
     Select the RF frequency (Region CN470)  ---> 
     Select RF Modem (Modem LoRa)  --->
```

![image.png](../image/02_lora-radio-test-shell-cmdlist.png)

| 序号 | finish命令 | 说明 |
| --- | --- | --- |
| 1 | lora probe | 测试lora设备(SPI)访问是否正常 |
| 2 | lora cw <para1> <para2> | 输出CW,可用于测试发射功率、频点等<br />\<para1\>:频点，单位Hz<br>\<para2\>:功率，单位dBm |
| 3 | lora ping <para1> <para2> | 单向\双向通信测试<br />\<para1\> : 主机\从机<br>-m 主机<br>-s 从机<br> \<para2\>: 发送数据包个数 |
| 4 | lora rx  | 接收(监听)数据包，同时以16进制格式与ASCII码显示数据内容 |
| 5 | lora config <para1> <para2> | 配置射频参数<br />\<para1\>:radio参数，字符表示<br/>  freq 表示频率，单位Hz<br/>  power 表示发射功率，单位dbm<br/>  sf 表示扩频因子，有效值: 7~12<br/>  bw表示带宽，有效值: 0 (125kHz)、1 (250KHz)、2 (500KHz)<br/>  public表示同步字，有效值: 0 （sync = 0x12), 1  (sync = 0x34)<br/>  iq 表示iq反转，有效值: 0 (iq不反转)，1 (iq反转)<br/>\<para2\>:radio参数的具体值 |

![image.png](../image/03_lora-ping_SX1278-SX1268-TRX-test.png)
lora ping 双向通信测试示例(SX1278 <-> SX1268)
![image.png](../image/04_lora-rx_sniffer-test.png)
lora rx 单向接收(监听)lora数据包测试示例 (SX1278 <- 或-> SX1268)

## 4.3 应用层调用说明

用户层调用可以参考如下步骤

1. 定义射频DIO中断服务回调函数

```c

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

```

1. 调用lora-radio初始化

```c
void main(void)
{
    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    if(Radio.Init(&RadioEvents))
    {
        Radio.SetPublicNetwork( false );
        lora_chip_initialized = true;
    }
    //.....
}
```

1. 配置射频通信参数

```c
{
    Radio.SetChannel( lora_radio_test_paras.frequency );

    if( lora_radio_test_paras.modem == MODEM_LORA )
    {
        Radio.SetTxConfig( MODEM_LORA, lora_radio_test_paras.txpower, 0, lora_radio_test_paras.bw,
                          lora_radio_test_paras.sf, lora_radio_test_paras.cr,
                          LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON_DISABLE,
                          true, 0, 0, LORA_IQ_INVERSION_ON_DISABLE, 3000 );

        Radio.SetRxConfig( MODEM_LORA, lora_radio_test_paras.bw, lora_radio_test_paras.sf,
                          lora_radio_test_paras.cr, 0, LORA_PREAMBLE_LENGTH,
                          LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON_DISABLE,
                          0, true, 0, 0, LORA_IQ_INVERSION_ON_DISABLE, true );
    }
}
```

1. 数据发送

```c
Radio.Send( Buffer, len );
```

1. 数据接收

```c
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Sleep( );
    BufferSize = size;
    rt_memcpy( Buffer, payload, BufferSize );
    rssi_value = rssi;
    snr_value = snr;
    // .....
}
```
