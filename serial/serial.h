/*
 * serial.h
 *
 *  Created on: Jan. 17, 2022
 *      Author: braed
 */

#include "aaaa.h"
#include "cpu.h"


#ifndef SERIAL_H_
#define SERIAL_H_

// RCG address
#define UART_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS          0x400fe618;

// macros for RCG bitfields
#define UART_ZERO_ENABLE_BIT                                0UL
#define UART_ONE_ENABLE_BIT                                 1UL
#define UART_TWO_ENABLE_BIT                                 2UL
#define UART_THREE_ENABLE_BIT                               3UL
#define UART_FOUR_ENABLE_BIT                                4UL
#define UART_FIVE_ENABLE_BIT                                5UL
#define UART_SIX_ENABLE_BIT                                 6UL
#define UART_SEVEN_ENABLE_BIT                               7UL

// start address of UART registers - note they are consecutive - could make a struct array
#define UART_ZERO_REGS_START_ADDR                                0x4000C000;
#define UART_ONE_REGS_START_ADDR                                 0x4000D000;
#define UART_TWO_REGS_START_ADDR                                 0x4000E000;
#define UART_THREE_REGS_START_ADDR                               0x4000F000;
#define UART_FOUR_REGS_START_ADDR                                0x40010000;
#define UART_FIVE_REGS_START_ADDR                                0x40011000;
#define UART_SIX_REGS_START_ADDR                                 0x40012000;
#define UART_SEVEN_REGS_START_ADDR                               0x40013000;

// Baud rates
// Integer portion
#define UART_CLOCK_DIV                       16
#define BAUD_115200_INT             (uint32)(( SYS_CLOCK_HZ ) / (UART_CLOCK_DIV * 115200))
#define BAUD_115200_FRACT            (uint32)(((float32)(((float32)(SYS_CLOCK_HZ ) / (float32)(UART_CLOCK_DIV * 115200)) \
                                                - BAUD_115200_INT) * 64.0f) + 0.5)

// Data length
#define EIGHT_BITS            0x3
#define BIT_LENGTH_SHIFT      5
#define FIFO_ENABLE_SHIFT     4

//Uart Flag bits
#define UART_BUSY             3
#define UART_RECEIVED         4

// Uart ID Range, Stop Byte, Start Byte
#define UART_STOP             0xFA
#define UART_START            0xAF
#define UART_ID_MAX           0x0A

#define UART_DATA_ERROR_MASK 0xF00

struct uart_store
{
    uint16 uart_receive_id; // type and ID for the value
    void (*uart_store_function)(void);
};

enum uart_channels
{
 UART_ZERO,
 UART_ONE,
 UART_TWO,
 UART_THREE,
 UART_FOUR,
 UART_FIVE,
 UART_SIX,
 UART_SEVEN,
 MAX_UARTS,
};

enum uart_states
{
    UART_IDLE,
    UART_RECEIVE,
    UART_TRANSMIT,
    UART_MESSAGE_RECEIVED,
};


struct uart_regs
{
    uint32 uart_data_register;
    uint32 uart_receive_status_error_clear;
    uint32 uart_reserved1[4];
    uint32 uart_flag;
    uint32 uart_reserved2;
    uint32 uart_irda_low_power_reg;
    uint32 uart_int_baud_rate_divisor;
    uint32 uart_fract_baud_rate_divisor;
    uint32 uart_line_control;
    uint32 uart_control;
    uint32 uart_interrupt_fifo_select;
    uint32 uart_interrupt_mask;
    uint32 uart_raw_inerrupt_status;
    uint32 uart_masked_interrupt_status;
    uint32 uart_interrupt_clear;
    uint32 uart_dma_control;
    uint32 uart_reserved3[22];
    uint32 uart_9bit_self_address;
    uint32 uart_9bit_self_address_mask;
    uint32 uart_reserved4[965];
    uint32 uart_peripheral_properties;
    uint32 uart_reserved5;
    uint32 uart_clock_configuration;
    uint32 uart_reserved6;
    uint32 uart_periph_id_4;
    uint32 uart_periph_id_5;
    uint32 uart_periph_id_6;
    uint32 uart_periph_id_7;
    uint32 uart_periph_id_0;
    uint32 uart_periph_id_1;
    uint32 uart_periph_id_2;
    uint32 uart_periph_id_3;
    uint32 uart_prime_cell_id_0;
    uint32 uart_prime_cell_id_1;
    uint32 uart_prime_cell_id_2;
    uint32 uart_prime_cell_id_3;
};


extern void uart_clock_enable(enum uart_channels uart_number);
extern void uart_configure(enum uart_channels uart_number);
extern void uart0_configure_for_test(enum uart_channels uart_number);
extern void uart_service(void);


#endif /* SERIAL_H_ */
