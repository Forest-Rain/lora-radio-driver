/*
 * This file is only used for doxygen document generation.
 */

/*!
 * @mainpage Introduce
 * @author wsn-hxj
 * @version v1.5.0
 * @date 2021-11-15
 * 
 * lora-radio-driver软件包是基于RTOS( RT-Thread ) 实现的LoRa Tranceiver芯片(SX126x、SX127x等)的驱动文件，该驱动文件通过SPI读写LoRa Tranceiver芯片，可用于快速搭建基于LoRa等通信的应用产品。
 * 主要功能如下:
 *  - 1. 当前支持LoRa Transceiver（sx126x\sx127x等）
 *   - 支持Sub-1GHz频段
 *   - 支持LoRa调制方式(SF5~SF12、BW125~BW500)
 *   - 支持FSK调制方式
 *  - 2. 可作为phy层对接到LoRaWAN End-Device协议栈
 *  - 3. 提供常用实例代码tester，可用于射频性能测试、空口数据包监听、双向通信测试等
 *  - 4. 当前支持的LoRa 模块\芯片，如
 *   - LSD4RF-2R717N40 (SX1268 CN470频段)
 *   - LSD4RF-2R822N300 (SX1262 868/915MHz频段)
 *   - LSD4RF-2F717N30(SX1278 CN470频段)
 *   - Ra-01(SX1278 CN470频段)等等
 * @section application_arch 01 功能框图
 * 
 * lora-radio-driver功能框图:
 * @image html lora-radio-driver_func_arch.png "Figure 1: 功能框图"
 *
 * @section file_arch 02 文件结构
 * 
 * lora-radio-driver文件结构:
 * @image html lora-radio-driver_file_arch.png "Figure 1: 文件结构"
 * 
 * @section lora-radio-driver 03 用户接口API
 * 更多详细信息，请参考 @ref LORA_RADIO_UPPER_API
 */





