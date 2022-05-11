/*
 * serial.c
 *
 *  Created on: Jan. 17, 2022
 *      Author: braed
 */

#include "serial/serial.h"
#include "gpio/gpio.h"
#include "aaaa.h"
#include "application.h"

static struct gpio_regs *gpio_regs;
static struct uart_regs *uart_regs[(int16)MAX_UARTS];
static enum uart_states uart_state;

// first byte for message ID, second for parameter value
// 3, 4, 5, 6 are the value. Byte 7 is the stop byte.
static volatile uint32 receive[10];
static volatile uint32 index;
static uint32 data_error;
volatile uint32 error_check, start_check;
static uint32 store_size;

local uint32 count;

// Memory-mapped UART registers
//#pragma DATA_SECTION(uart_regs, ".uart")
//global volatile struct uart_regs uart_regs;

////////////////////////////////////////////////////////////////////////////////////////////
//
// Local Functions
//
////////////////////////////////////////////////////////////////////////////////////////////

local void test_function_store(uint32 arg)
{

    test_function_store_application(arg);

}

//
// updates PI data when requested
//
local void send_data_to_PI(uint32 arg)
{
    uart_regs[UART_TWO]->uart_data_register = 0xAF;
    uart_regs[UART_TWO]->uart_data_register = 0x02;
    uart_regs[UART_TWO]->uart_data_register = (count >> 24) & 0xFF;
    uart_regs[UART_TWO]->uart_data_register = (count >> 16) & 0xFF;
    uart_regs[UART_TWO]->uart_data_register = (count >> 8) & 0xFF;
    uart_regs[UART_TWO]->uart_data_register = count & 0xFF;
    count++;
    uart_regs[UART_TWO]->uart_data_register = 0xFA;
}

const struct uart_store uart_store_array[] =
{
     // Don't skip any ID so that indexes correspond to id number
     {0x00,  NULL                },
     {0x01,  NULL                },
     {0x02,  &test_function_store},
     {0x03,  &send_data_to_PI    },
     {0x04,  NULL                },
};



//
// Services receive message array
//
//local void uart_message_array(void)
//{
//    int32 i;
//    for(i = 0; i < store_size; i++)
//    {
//        if(uart_store_array[i].uart_receive_id == receive[1])
//        {
//            uart_store_array[i].uart_store_function();
//        }
//    }
//}





////////////////////////////////////////////////////////////////////////////////////////////
//
// Global Functions
//
////////////////////////////////////////////////////////////////////////////////////////////

//
// Enables the run time clock gating for given uart channel
//
global void uart_clock_enable(enum uart_channels uart_number)
{
    if(uart_number > UART_SEVEN)
        return; // TODO: replace with exit_crash

    uint32 *uart_enable = (uint32*)UART_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS;
    *uart_enable |= (1 << uart_number);
}

//
// Configures the uart for interrupts
//
global void uart_configure_interrupts(enum uart_channels uart_number)
{
    // Set interrupts levels on FIFOs
    // Clears bits
    uart_regs[(int32)uart_number]->uart_interrupt_fifo_select &= 0xFFC0;
    // Sets Tx FIFO to 1/2 full and Rx FIFO to 1/2 full
    uart_regs[(int32)uart_number]->uart_interrupt_fifo_select |= (0x1 | (1 << 3)); //1 is 1/4, 2 is 1/2;

    // Set interrupt mask
    uart_regs[(int32)uart_number]->uart_interrupt_mask |= ((0x1 << UART_IM_RXIM) | (0x1 << UART_IM_TXIM));

    // Enable interrupt for UART2 in the NVIC - disable interrupts to NVIC when using dma
    //uint32 *pointer = (uint32*)INTERRUPTS_32_to_63;
    //*pointer |= (1 << 1);
}

//
// Configures the uart - only supports UART_TWO at this time
//
global void uart_configure(enum uart_channels uart_number)
{

    if(uart_number == UART_TWO) // TODO: add support for other UART channels
    {
        // set drive strength to 8mA and enable variable slew rate
        gpio_regs = (struct gpio_regs *)GPIO_REGS_PORTD;
        gpio_regs->gpio_8mA_drive |= (0x3 << 6);
        gpio_regs->gpio_slew_rate |= (0x3 << 6);
        gpio_regs->gpio_pull_up_select |= (0x3 << 6);
        gpio_regs->gpio_digital_enable |= (0x3 << 6);

    }

    uart_regs[(int32)UART_ZERO] = (struct uart_regs *)UART_ZERO_REGS_START_ADDR;
    uart_regs[(int32)UART_ONE] = (struct uart_regs *)UART_ONE_REGS_START_ADDR;
    uart_regs[(int32)UART_TWO] = (struct uart_regs *)UART_TWO_REGS_START_ADDR ;
    uart_regs[(int32)UART_THREE] = (struct uart_regs *)UART_THREE_REGS_START_ADDR;
    uart_regs[(int32)UART_FOUR] = (struct uart_regs *)UART_FOUR_REGS_START_ADDR;
    uart_regs[(int32)UART_FIVE] = (struct uart_regs *)UART_FIVE_REGS_START_ADDR;
    uart_regs[(int32)UART_SIX] = (struct uart_regs *)UART_SIX_REGS_START_ADDR;
    uart_regs[(int32)UART_SEVEN] = (struct uart_regs *)UART_SEVEN_REGS_START_ADDR;

    // Disable UART
    uart_regs[(int32)uart_number]->uart_control &= 0xFFFFFFFE;
    //Set Baud Rate
    uart_regs[(int32)uart_number]->uart_int_baud_rate_divisor = BAUD_115200_INT;
    uart_regs[(int32)uart_number]->uart_fract_baud_rate_divisor = BAUD_115200_FRACT;
    // Set data length, parity and stop bit. Enable FIFO buffers
    uart_regs[(int32)uart_number]->uart_line_control |= (EIGHT_BITS << BIT_LENGTH_SHIFT ) | (1 << FIFO_ENABLE_SHIFT );
    // Set DMA
    uart_regs[(int32)uart_number]->uart_dma_control |= 0x3; //Enable Tx DMA and Rx DMA
    //Enable UART
    uart_regs[(int32)uart_number]->uart_control |= 0x301; //enable UART, Tx, Rx enable

    // FIFOs intterupt at 1/4 full ( max is 16 bytes )
    //uart_regs[(int32)uart_number]->uart_interrupt_fifo_select |= (0x1 | (0x1 << 3));//(0x2 | (0x2 << 3));

    uart_state = UART_IDLE;
    count = 0UL;
    data_error = 0;
    store_size = sizeof( uart_store_array)/sizeof(uart_store_array[0]);
}

//
// COnfigure UART0 for testing
//
//
// Configures the uart - only supports UART_TWO at this time
//
global void uart0_configure_for_test(enum uart_channels uart_number)
{

    uart_regs[(int32)UART_ZERO] = (struct uart_regs *)UART_ZERO_REGS_START_ADDR;


    // Disable UART
    uart_regs[(int32)uart_number]->uart_control &= 0xFFFFFFFE;
    //Set Baud Rate
    uart_regs[(int32)uart_number]->uart_int_baud_rate_divisor = BAUD_115200_INT;
    uart_regs[(int32)uart_number]->uart_fract_baud_rate_divisor = BAUD_115200_FRACT;
    // Set DMA
    uart_regs[(int32)uart_number]->uart_dma_control |= 0x3; //Enable Tx DMA and Rx DMA
    // set 9bit mask
    uart_regs[(int32)uart_number]->uart_9bit_self_address_mask |= 0xff;
    uart_regs[(int32)uart_number]->uart_interrupt_fifo_select |= 0x12;
    uart_regs[(int32)uart_number]->uart_raw_inerrupt_status |= 0xf;
    // Set data length, parity and stop bit. Enable FIFO buffers
    uart_regs[(int32)uart_number]->uart_line_control |= (EIGHT_BITS << BIT_LENGTH_SHIFT ) | (1 << FIFO_ENABLE_SHIFT );
    //Enable UART
    uart_regs[(int32)uart_number]->uart_control |= 0x380;
    uart_regs[(int32)uart_number]->uart_control |= 0x1;     //enable UART, Tx, Rx enable

}

//
// Respond to message
//
global bool uart_rx_service(enum uart_channels uart_number, uint32 id, uint32 arg)
{
    if((uart_regs[(int32)UART_TWO]->uart_receive_status_error_clear & 0xf) > 0)
        return false;

    if(uart_store_array[id].uart_receive_id == id)
        uart_store_array[id].uart_store_function(arg);
    else
        return false;


    return true;


}

//
// Services the serial comms - this was pre DMA. Now unused
//
//global void uart_service(void)
//{
//
//
//    uint32 no_char = uart_regs[(int32)UART_TWO]->uart_flag;
//    no_char = ((no_char >> UART_RECEIVED) & 0x1);
//
//    switch(uart_state)
//    {
//        case UART_IDLE:
//        {
//            if(!no_char)
//            {
//
//                index = 0;
//                start_check = HWREG(0x4000E000);
//
//                if(start_check == UART_START)
//                {
//                    uart_state = UART_RECEIVE;
//                    receive[index] = start_check;
//                    index++;
//                }
//
//                // if error, discard message and clear error bits
//                error_check = (start_check & UART_DATA_ERROR_MASK);
//                if(error_check > 0)
//                {
//                    uart_state = UART_IDLE;
//                    index = 0;
//                    uart_regs[(int32)UART_TWO]->uart_receive_status_error_clear |= 0xF;
//                }
//
//            }
//            break;
//        }
//
//
//        case UART_RECEIVE:
//        {
//            if(!no_char)
//            {
//                receive[index] = HWREG(0x4000E000);//uart_regs[(int32)UART_TWO]->uart_data_register
//                //uart_regs[(int32)UART_TWO]->uart_data_register = receive[index];
//
//
//                if(receive[index] == UART_STOP)
//                {
//                    uart_state = UART_MESSAGE_RECEIVED;
//                    index = 0;
//                    uart_regs[(int32)UART_TWO]->uart_data_register = 0x22;
//                }
//
//                // if error, discard message and clear error bits
//                error_check = (receive[index] & UART_DATA_ERROR_MASK);
//                if(error_check > 0)
//                {
//                    uart_state = UART_IDLE;
//                    index = 0;
//                    uart_regs[(int32)UART_TWO]->uart_receive_status_error_clear |= 0xF;
//                }
//
//                index++;
//            }
//
//
//            break;
//        }
//        case UART_TRANSMIT:
//        {
//            break;
//        }
//
//        case UART_MESSAGE_RECEIVED:
//        {
//            uart_message_array();
//            uart_state = UART_IDLE;
//            index = 0;
//            break;
//        }
//
//        default:
//        {
//            uart_state = UART_IDLE;
//            index = 0;
//        }
//
//    }
//
//}



