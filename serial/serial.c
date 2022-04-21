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
static uint32 full;
volatile uint32 error_check, start_check;
static uint32 store_size;

// Memory-mapped UART registers
//#pragma DATA_SECTION(uart_regs, ".uart")
//global volatile struct uart_regs uart_regs;

////////////////////////////////////////////////////////////////////////////////////////////
//
// Local Functions
//
////////////////////////////////////////////////////////////////////////////////////////////

local void test_function_store(void)
{
    //retrieve value sent from Pi
    uint32 value_received = 0;

    value_received |= (receive[2]<<(24));
    value_received |= (receive[3]<<(16));
    value_received |= (receive[4]<<(8));
    value_received |= (receive[5]);

    test_function_store_application(value_received);

}

const struct uart_store uart_store_array[] =
{
     {0x00,  NULL                },
     {0x01,  NULL                },
     {0x02,  &test_function_store},
     {0x03,  NULL                },
     {0x04,  NULL                },
};



//
// Services receive message array
//
local void uart_message_array(void)
{
    int32 i;
    for(i = 0; i < store_size; i++)
    {
        if(uart_store_array[i].uart_receive_id == receive[1])
        {
            uart_store_array[i].uart_store_function();
        }
    }
}





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

    uart_state = UART_IDLE;
    data_error = 0;
    full = 0;
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
// Services the serial comms
//
global void uart_service(void)
{
//    uint32 busy = uart_regs[(int32)UART_TWO]->uart_flag;
//    busy = ((busy >> UART_BUSY) & 0x1);

    uint32 no_char = uart_regs[(int32)UART_TWO]->uart_flag;
    no_char = ((no_char >> UART_RECEIVED) & 0x1);
//    full = ((no_char >> 6) & 0x1);
//
//    if(full)
//        full = 1;
  //  index = 0;

    switch(uart_state)
    {
        case UART_IDLE:
        {
            if(!no_char)
            {

                index = 0;
                start_check = HWREG(0x4000E000);
                //uart_regs[(int32)UART_TWO]->uart_data_register = start_check;

                if(start_check == UART_START)
                {
                    uart_state = UART_RECEIVE;
                    receive[index] = start_check;
                    index++;
                }

                // if error, discard message and clear error bits
                error_check = (start_check & UART_DATA_ERROR_MASK);
                if(error_check > 0)
                {
                    uart_state = UART_IDLE;
                    index = 0;
                    uart_regs[(int32)UART_TWO]->uart_receive_status_error_clear |= 0xF;
                }

            }
            break;
        }


        case UART_RECEIVE:
        {
            if(!no_char)
            {
                receive[index] = HWREG(0x4000E000);//uart_regs[(int32)UART_TWO]->uart_data_register
                //uart_regs[(int32)UART_TWO]->uart_data_register = receive[index];


                if(receive[index] == UART_STOP)
                {
                    uart_state = UART_MESSAGE_RECEIVED;
                    index = 0;
                    uart_regs[(int32)UART_TWO]->uart_data_register = 0x22;
                }

                // if error, discard message and clear error bits
                error_check = (receive[index] & UART_DATA_ERROR_MASK);
                if(error_check > 0)
                {
                    uart_state = UART_IDLE;
                    index = 0;
                    uart_regs[(int32)UART_TWO]->uart_receive_status_error_clear |= 0xF;
                }

                index++;
            }


            break;
        }
        case UART_TRANSMIT:
        {
            break;
        }

        case UART_MESSAGE_RECEIVED:
        {
            uart_message_array();
            uart_state = UART_IDLE;
            index = 0;
            break;
        }

        default:
        {
            uart_state = UART_IDLE;
            index = 0;
        }

    }


//    if(!busy)
//    {
//        //uart_regs[(int32)UART_TWO]->uart_data_register = 0x2c;
//        uint32 *pointer = (uint32*)0x4000E000;
//        *pointer |= 0x2c;
//    }
}



