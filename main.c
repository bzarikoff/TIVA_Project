
/*
 *  main function
 */

#include "init.h"
#include "aaaa.h"
#include "dma/dma.h"
#include "serial/serial.h"

//#pragma DATA_SECTION(uart_regs, ".uart")
//global volatile struct uart_regs uart_regs;

local uint32 count = 0;
static struct dma_regs *dma_regs;
static struct uart_regs *uart_regs[(int16)MAX_UARTS];
local uint32 *dma_int_status;


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

    dma_int_status = 0x400FF504; // location of dma intterupt status register

    initialization_routine();


  // uint32 test = uart_regs.uart_data_register;

    while(1)
    {
        //uart_service();
        dma_enable();

    }

}
