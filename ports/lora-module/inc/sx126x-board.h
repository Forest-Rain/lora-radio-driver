/*!
 * \file      sx126x-board.h
 *
 * \brief     Target board SX126x driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __SX126x_BOARD_H__
#define __SX126x_BOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include "sx126x/sx126x.h"
#include "rtconfig.h"

#ifdef LORA_RADIO_GPIO_SETUP_BY_PIN_NAME
    #define LORA_RADIO_NSS_PIN       stm32_pin_get(LORA_RADIO_NSS_PIN_NAME)
    #define LORA_RADIO_BUSY_PIN      stm32_pin_get(LORA_RADIO_BUSY_PIN_NAME)
    #define LORA_RADIO_DIO1_PIN      stm32_pin_get(LORA_RADIO_DIO1_PIN_NAME)
    #define LORA_RADIO_RESET_PIN     stm32_pin_get(LORA_RADIO_RESET_PIN_NAME)
    #if defined( LORA_RADIO_DIO2_PIN_NAME ) 
    #define LORA_RADIO_DIO2_PIN      stm32_pin_get(LORA_RADIO_DIO2_PIN_NAME)
    #endif
    #if defined( LORA_RADIO_RFSW1_PIN_NAME ) && defined ( LORA_RADIO_RFSW2_PIN_NAME )  
    #define LORA_RADIO_RFSW1_PIN     stm32_pin_get(LORA_RADIO_RFSW1_PIN_NAME)
    #define LORA_RADIO_RFSW2_PIN     stm32_pin_get(LORA_RADIO_RFSW2_PIN_NAME)
    #endif
#else
    // ��δʹ��menuconfig,�ɸ���ʵ��ʹ�õ�LoRaģ�飬ֱ�����øò���
    #ifndef LORA_RADIO_NSS_PIN
    #define LORA_RADIO_NSS_PIN    GET_PIN(A,15)
    #endif
    #ifndef LORA_RADIO_RESET_PIN
    #define LORA_RADIO_RESET_PIN  GET_PIN(A,7)
    #endif
    #ifndef LORA_RADIO_DIO1_PIN
    #define LORA_RADIO_DIO1_PIN   GET_PIN(B,1)
    #endif
    #ifndef LORA_RADIO_BUSY_PIN
    #define LORA_RADIO_BUSY_PIN   GET_PIN(B,2)
    #endif
    #ifndef LORA_RADIO_RFSW1_PIN
    #define LORA_RADIO_RFSW1_PIN   GET_PIN(B,0)
    #endif
    #ifndef LORA_RADIO_RFSW2_PIN
    #define LORA_RADIO_RFSW2_PIN   GET_PIN(C,5)
    #endif
#endif // end of LORA_RADIO_GPIO_SETUP_BY_PIN_NAME

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME                          2

#define DelayMs( ms )                                   rt_thread_mdelay(ms) 

/*!
 * \brief Initializes the radio I/Os pins interface
 */
void SX126xIoInit( void );

/*!
 * \brief Initializes DIO IRQ handlers
 *
 * \param [IN] irqHandlers Array containing the IRQ callback functions
 */
void SX126xIoIrqInit( DioIrqHandler dioIrq );

/*!
 * \brief De-initializes the radio I/Os pins interface.
 *
 * \remark Useful when going in MCU low power modes
 */
void SX126xIoDeInit( void );

/*!
 * \brief Initializes the TCXO power pin.
 */
void SX126xIoTcxoInit( void );

/*!
 * \brief HW Reset of the radio
 */
void SX126xReset( void );

/*!
 * \brief Blocking loop to wait while the Busy pin in high
 */
void SX126xWaitOnBusy( void );

/*!
 * \brief Wakes up the radio
 */
void SX126xWakeup( void );

/*!
 * \brief Send a command that write data to the radio
 *
 * \param [in]  opcode        Opcode of the command
 * \param [in]  buffer        Buffer to be send to the radio
 * \param [in]  size          Size of the buffer to send
 */
void SX126xWriteCommand( RadioCommands_t opcode, uint8_t *buffer, uint16_t size );

/*!
 * \brief Send a command that read data from the radio
 *
 * \param [in]  opcode        Opcode of the command
 * \param [out] buffer        Buffer holding data from the radio
 * \param [in]  size          Size of the buffer
 *
 * \retval status Return command radio status
 */
uint8_t SX126xReadCommand( RadioCommands_t opcode, uint8_t *buffer, uint16_t size );

/*!
 * \brief Write a single byte of data to the radio memory
 *
 * \param [in]  address       The address of the first byte to write in the radio
 * \param [in]  value         The data to be written in radio's memory
 */
void SX126xWriteRegister( uint16_t address, uint8_t value );

/*!
 * \brief Read a single byte of data from the radio memory
 *
 * \param [in]  address       The address of the first byte to write in the radio
 *
 * \retval      value         The value of the byte at the given address in radio's memory
 */
uint8_t SX126xReadRegister( uint16_t address );

/*!
 * \brief Sets the radio output power.
 *
 * \param [IN] power Sets the RF output power
 */
void SX126xSetRfTxPower( int8_t power );

/*!
 * \brief Initializes the RF Switch I/Os pins interface
 */
void SX126xAntSwOn( void );

/*!
 * \brief De-initializes the RF Switch I/Os pins interface
 *
 * \remark Needed to decrease the power consumption in MCU low power modes
 */
void SX126xAntSwOff( void );

/*!
 * \brief Set the RF Switch I/Os pins interface
 */
void SX126xSetAntSw( RadioOperatingModes_t mode );

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param [IN] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
bool SX126xCheckRfFrequency( uint32_t frequency );

/*!
 * \brief Gets the Defines the time required for the TCXO to wakeup [ms].
 *
 * \retval time Board TCXO wakeup time in ms.
 */
uint32_t SX126xGetBoardTcxoWakeupTime( void );

/*!
 * \brief Writes new Tx debug pin state
 *
 * \param [IN] state Debug pin state
 */
void SX126xDbgPinTxWrite( uint8_t state );

/*!
 * \brief Writes new Rx debug pin state
 *
 * \param [IN] state Debug pin state
 */
void SX126xDbgPinRxWrite( uint8_t state );


#endif // __SX126x_BOARD_H__
