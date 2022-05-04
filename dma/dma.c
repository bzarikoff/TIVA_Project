/*
 * dma.c
 *
 *  Created on: Feb. 9, 2022
 *      Author: braed
 */
#include "aaaa.h"
#include "dma/dma.h"
#include "serial/serial.h"

static struct dma_channel_control *dma_chan_ctrl;
static struct dma_regs *dma_regs;
static uint32 *channel_control_table;
static uint32 RxBuffer[12];
static uint8 TxBuffer[12];
global uint32 dma_count;

//
// Initializes dma channel control table
//
local void dma_channel_control_table_init(void)
{

    *(dma_regs->dma_channel_ctl_base_ptr + 0UL) = (uint32)UART_TWO_REGS_START_ADDR;// send back what it receives
    *(dma_regs->dma_channel_ctl_base_ptr + 1UL) = &(RxBuffer[6]);//(uint32)(0x20000C00);
    *(dma_regs->dma_channel_ctl_base_ptr + 2UL) = (uint32)(0x8C000001);//(uint32)(0xC0000002);
    *(dma_regs->dma_channel_ctl_base_ptr + 2UL) |= (uint32)(0x2 << 14); // 2 is 4, 3 is 8 transfers, 4 ia 16
    *(dma_regs->dma_channel_ctl_base_ptr + 2UL) |= (uint32)(6UL << 4); // transfer size is 7
    //*(dma_regs->dma_channel_ctl_base_ptr + 2UL) |= (uint32)(0x7 << 4);
    *(dma_regs->dma_channel_ctl_base_ptr + 3UL) = (uint32)(0x00000000);//(uint32)(0x0C000002);

    *(dma_regs->dma_channel_ctl_base_ptr + 4UL) = &(TxBuffer[11]);//(uint32)(0x20001C00);
    *(dma_regs->dma_channel_ctl_base_ptr + 5UL) = (uint32)UART_TWO_REGS_START_ADDR;
    *(dma_regs->dma_channel_ctl_base_ptr + 6UL) = (uint32)(0xCC000001);//(uint32)(0xC0000002);
    *(dma_regs->dma_channel_ctl_base_ptr + 7UL) = (uint32)(0x00000000);//(uint32)(0xC0000002);

    uint16 index;
    for(index = 0; index < 12; index++)
    {
        RxBuffer[index] = 0x0;
        TxBuffer[index] = 0x0;
    }

    uint32 *pointer = (uint32*)0x20000C00;
    *pointer = 0UL;

    uint16 i = 8;
    while(i < 128)
    {
        *(dma_regs->dma_channel_ctl_base_ptr + i) = (uint32)(0x00000000);
        i++;
    }

    // test a software DMA request
//    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL)) = &(RxBuffer[0]); //  Rx is source
//    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL + 1)) = &(TxBuffer[11]);; // Tx is destination
//    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL + 2)) = (uint32)(0x0C000002);//(uint32)(0xC0000002);
//    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL + 3)) = (uint32)(0x00000000);//(uint32)(0xC0000002);
//    dma_count = 0UL;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                Global Functions                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////


//
// Enable clock for DMA
//
global void dma_clock_enable(void)
{
    uint32 *clock = (uint32*)DMA_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS;
    *clock |= 0x1;
}

//
// Initializes DMA
//
global void dma_initialize(void)
{
    dma_regs = (struct dma_regs*)DMA_REGISTERS_BASE_ADDRESS;
    dma_regs->dma_config |= 0x1;
    dma_regs->dma_channel_map_select_0 = ((0x1) | (0x1 << 4)); // Channel 0 UART2 Rx, Channel 1 UART2 Tx
    dma_regs->dma_channel_ctl_base_ptr = (uint32*)0x20002000;
    dma_regs->dma_channel_priority_set |= (uint32)0x1;
    dma_regs->dma_primary_alt_clear |= (uint32)((1 << 6)|| 0x1);
   // dma_regs->dma_channel_enable_set |= (uint32)(1 << 6);
   // dma_regs->dma_channel_req_mask_set |= (uint32)0xFFBF;//(uint32)0xFFFC;//enables requests for only CH0 and CH1 and CH6(SW)
    dma_regs->dma_channel_req_mask_set |= (uint32)0xFFFE;
    dma_regs->dma_channel_useburst_set |= (uint32)0x1; //  burst

    dma_channel_control_table_init();

    dma_regs->dma_channel_enable_set |= (uint32)0x1;//enables CH0 only for now (Rx)
}


//
//
//
global void dma_service(void)
{

    // if dma transfer complete, reset register and prepare for next transfer
    if((dma_regs->dma_channel_intr_status & 0x1) == 0x1)
    {
        uint32 *pointer = UART_TWO_REGS_START_ADDR;
        uint32 junk_data;
        uint32 Rx_valid; // checks if dma is triggered due to Tx completion or Receive
        bool success;

        // Check if Rx buffer has characters
        if(RxBuffer[6] != 0)
        {
            // Check if message framed correctly
            if((RxBuffer[0] == 0xAF) && (RxBuffer[6] == 0xFA))
            {
                // format data for uart
                uint32 id = RxBuffer[1];
                uint32 arg = 0;
                arg =  (RxBuffer[2] << 24) | (RxBuffer[3] << 16) | (RxBuffer[4] << 8) | RxBuffer[5];
                success = uart_rx_service(UART_TWO, id, arg);
            }

            //data is incorrect - flush Rx uart
            else
            {
                junk_data = *pointer;

                while(junk_data!=0)
                    junk_data = *pointer;
            }

            uint16 index = 0;
            // clear receive buffer
            while(index < 7)
            {
                RxBuffer[index] = 0;
                index++;
            }

        }


        // rearm DMA
        dma_count++;
        if(success)
            *(pointer) = 0xff; // send acknowledge (success)
        else
            *(pointer) = 0xee; // send acknowledge (error)

        *(dma_regs->dma_channel_ctl_base_ptr + 2UL) |= (uint32)(0x1); //2 for CH0 0x2 for auto, 1 for basic
        dma_regs->dma_channel_enable_set |= (uint32)0x41;//enables CH0 only for now (Rx)
        dma_regs->dma_channel_useburst_set |= (uint32)0x1; // burst
        dma_regs->dma_channel_intr_status |= 0x1;
        *(dma_regs->dma_channel_ctl_base_ptr + 2UL) |= (uint32)(6UL << 4); // transfer size is 7
    }



}
