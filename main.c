
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


void UART2_ISR(void)
{
    count++;

    if(count > 10000UL)
        count = 0UL;

}

//void UART1IntHandler(void)
//{
//    uint32_t ui32Status;
//    uint32_t ui32Mode;
//
//    //
//    // Read the interrupt status of the UART.
//    //
//    //ui32Status = MAP_UARTIntStatus(UART1_BASE, 1);
//    ui32Status = uart_regs[(int32)UART_ZERO]->uart_receive_status_error_clear;
//
//    //
//    // Clear any pending status, even though there should be none since no UART
//    // interrupts were enabled.  If UART error interrupts were enabled, then
//    // those interrupts could occur here and should be handled.  Since uDMA is
//    // used for both the RX and TX, then neither of those interrupts should be
//    // enabled.
//    //
//    //MAP_UARTIntClear(UART1_BASE, ui32Status);
//    uart_regs[(int32)UART_ZERO]->uart_receive_status_error_clear &= 0x0;
//
//    //
//    // Check the DMA control table to see if the ping-pong "A" transfer is
//    // complete.  The "A" transfer uses receive buffer "A", and the primary
//    // control structure.
//    //
//    ui32Mode = MAP_uDMAChannelModeGet(UDMA_CHANNEL_UART1RX | UDMA_PRI_SELECT);
//
//    //
//    // If the primary control structure indicates stop, that means the "A"
//    // receive buffer is done.  The uDMA controller should still be receiving
//    // data into the "B" buffer.
//    //
//    if(ui32Mode == UDMA_MODE_STOP)
//    {
//        //
//        // Increment a counter to indicate data was received into buffer A.  In
//        // a real application this would be used to signal the main thread that
//        // data was received so the main thread can process the data.
//        //
//        g_ui32RxBufACount++;
//
//        //
//        // Set up the next transfer for the "A" buffer, using the primary
//        // control structure.  When the ongoing receive into the "B" buffer is
//        // done, the uDMA controller will switch back to this one.  This
//        // example re-uses buffer A, but a more sophisticated application could
//        // use a rotating set of buffers to increase the amount of time that
//        // the main thread has to process the data in the buffer before it is
//        // reused.
//        //
//        MAP_uDMAChannelTransferSet(UDMA_CHANNEL_UART1RX | UDMA_PRI_SELECT,
//                                   UDMA_MODE_PINGPONG,
//                                   (void *)(UART1_BASE + UART_O_DR),
//                                   g_ui8RxBufA, sizeof(g_ui8RxBufA));
//    }
//
//    //
//    // Check the DMA control table to see if the ping-pong "B" transfer is
//    // complete.  The "B" transfer uses receive buffer "B", and the alternate
//    // control structure.
//    //
//    ui32Mode = MAP_uDMAChannelModeGet(UDMA_CHANNEL_UART1RX | UDMA_ALT_SELECT);
//
//    //
//    // If the alternate control structure indicates stop, that means the "B"
//    // receive buffer is done.  The uDMA controller should still be receiving
//    // data into the "A" buffer.
//    //
//    if(ui32Mode == UDMA_MODE_STOP)
//    {
//        //
//        // Increment a counter to indicate data was received into buffer A.  In
//        // a real application this would be used to signal the main thread that
//        // data was received so the main thread can process the data.
//        //
//        g_ui32RxBufBCount++;
//
//        //
//        // Set up the next transfer for the "B" buffer, using the alternate
//        // control structure.  When the ongoing receive into the "A" buffer is
//        // done, the uDMA controller will switch back to this one.  This
//        // example re-uses buffer B, but a more sophisticated application could
//        // use a rotating set of buffers to increase the amount of time that
//        // the main thread has to process the data in the buffer before it is
//        // reused.
//        //
//        MAP_uDMAChannelTransferSet(UDMA_CHANNEL_UART1RX | UDMA_ALT_SELECT,
//                                   UDMA_MODE_PINGPONG,
//                                   (void *)(UART1_BASE + UART_O_DR),
//                                   g_ui8RxBufB, sizeof(g_ui8RxBufB));
//    }
//
//    //
//    // If the UART1 DMA TX channel is disabled, that means the TX DMA transfer
//    // is done.
//    //
//    if(!MAP_uDMAChannelIsEnabled(UDMA_CHANNEL_UART1TX))
//    {
//        //
//        // Start another DMA transfer to UART1 TX.
//        //
//        MAP_uDMAChannelTransferSet(UDMA_CHANNEL_UART1TX | UDMA_PRI_SELECT,
//                                   UDMA_MODE_BASIC, g_ui8TxBuf,
//                                   (void *)(UART1_BASE + UART_O_DR),
//                                   sizeof(g_ui8TxBuf));
//
//        //
//        // The uDMA TX channel must be re-enabled.
//        //
//        MAP_uDMAChannelEnable(UDMA_CHANNEL_UART1TX);
//    }
//}



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
    initialization_routine();


  // uint32 test = uart_regs.uart_data_register;

    while(1)
    {
        uart_service();
        //dma_enable();

    }

}
