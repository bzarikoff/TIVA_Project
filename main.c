
/*
 *  main function
 */

#include "init.h"
#include "aaaa.h"
#include "dma/dma.h"
#include "serial/serial.h"
#include "cpu.h"

//#pragma DATA_SECTION(uart_regs, ".uart")
//global volatile struct uart_regs uart_regs;

#define DMA_SERVICE_PERIOD        (uint32)((float32)(0.0001 ) * (float32)SYS_CLOCK_HZ) // period of 100us

local uint32 count = 0;
static struct dma_regs *dma_regs;
static struct uart_regs *uart_regs[(int16)MAX_UARTS];
local uint32 *dma_int_status;


//
// Unused since DMA added
//
void UART2_ISR(void)
{
    count++;


    if(*dma_int_status == 0x1)
        *dma_int_status |= 0x1; // clears dma interrupt

    // clears interrupt
    uart_regs[(int32)UART_TWO]->uart_interrupt_clear |= 0x10;


    if(count > 10000UL)
        count = 0UL;

}


//
// initialize timer used for main loop
//
local void timer_init(void)
{

    uint32 *pointer = SYSTICK_CONTROL_AND_STATUS;
    *(pointer + 1) = 0x00FFFFFF; // put max value in for reload register
    *pointer |= (0x1 | (1 << 2)); // enable timer, uses system clock
}

//
// read cpu timer value
//
local int32 cpu_timer_value(void)
{
    uint32 *pointer = SYSTICK_CURRENT_VAL;
    return (*pointer & 0x00FFFFFF);
}

//
// Expiry limit check
//
local int32 cpu_expiry_time(int32 cpu_timer, uint32 period)
{
    int32 diff = cpu_timer - period;

    if(diff < 0)
        return 0x00FFFFFC;

    else
        return diff;

}



void main(void)
{

    dma_regs = (struct dma_regs*)DMA_REGISTERS_BASE_ADDRESS;
    uart_regs[(int32)UART_ZERO] = (struct uart_regs *)UART_ZERO_REGS_START_ADDR;
    uart_regs[(int32)UART_ONE] = (struct uart_regs *)UART_ONE_REGS_START_ADDR;
    uart_regs[(int32)UART_TWO] = (struct uart_regs *)UART_TWO_REGS_START_ADDR ;
    uart_regs[(int32)UART_THREE] = (struct uart_regs *)UART_THREE_REGS_START_ADDR;
    uart_regs[(int32)UART_FOUR] = (struct uart_regs *)UART_FOUR_REGS_START_ADDR;
    uart_regs[(int32)UART_FIVE] = (struct uart_regs *)UART_FIVE_REGS_START_ADDR;
    uart_regs[(int32)UART_SIX] = (struct uart_regs *)UART_SIX_REGS_START_ADDR;
    uart_regs[(int32)UART_SEVEN] = (struct uart_regs *)UART_SEVEN_REGS_START_ADDR;

    dma_int_status = 0x400FF504; // location of dma interrupt status register

    initialization_routine();
    timer_init();

    int32 cpu_timer = (int32)cpu_timer_value();
    int32 dma_expiry = cpu_expiry_time(cpu_timer, DMA_SERVICE_PERIOD);



  // uint32 test = uart_regs.uart_data_register;

    while(1)
    {
        cpu_timer = (int32)cpu_timer_value();

        // Service period of 100us
        if(CPU_TIME_IS_AFTER(cpu_timer, dma_expiry))
        {
            dma_expiry = cpu_expiry_time(cpu_timer, DMA_SERVICE_PERIOD);
            dma_service();

        }


        //uart_service();
    }

}
