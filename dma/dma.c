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
static uint8 RxBuffer[12];
static uint8 TxBuffer[12];

//
// Enable clock for DMA
//
void dma_clock_enable(void)
{
    uint32 *clock = (uint32*)DMA_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS;
    *clock |= 0x1;
}

//
// Initializes dma channel control table
//
void dma_channel_control_table_init(void)
{

    *(dma_regs->dma_channel_ctl_base_ptr + 0UL) = &(RxBuffer[11]);//(uint32)(0x20000C00);//(uint32)UART_TWO_REGS_START_ADDR;
    *(dma_regs->dma_channel_ctl_base_ptr + 1UL) = (uint32)UART_TWO_REGS_START_ADDR;//(uint32)(0x20000C00);
    *(dma_regs->dma_channel_ctl_base_ptr + 2UL) = (uint32)(0x0C000001);
    *(dma_regs->dma_channel_ctl_base_ptr + 2UL) |= (uint32)(0x1 << 14);
    *(dma_regs->dma_channel_ctl_base_ptr + 3UL) = (uint32)(0x00000000);//(uint32)(0x0C000002);

    *(dma_regs->dma_channel_ctl_base_ptr + 4UL) = &(TxBuffer[11]);//(uint32)(0x20001C00);
    *(dma_regs->dma_channel_ctl_base_ptr + 5UL) = (uint32)UART_TWO_REGS_START_ADDR;
    *(dma_regs->dma_channel_ctl_base_ptr + 6UL) = (uint32)(0xCC000002);//(uint32)(0xC0000002);
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
    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL)) = &(RxBuffer[0]); //  Rx is source
    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL + 1)) = &(TxBuffer[11]);; // Tx is destination
    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL + 2)) = (uint32)(0x0C000002);//(uint32)(0xC0000002);
    *(dma_regs->dma_channel_ctl_base_ptr + (6*4UL + 3)) = (uint32)(0x00000000);//(uint32)(0xC0000002);
    (RxBuffer[0]) = 0x12;
    (RxBuffer[1]) = 0x34;
    (RxBuffer[2]) = 0x56;
    (RxBuffer[3]) = 0x78;
    (RxBuffer[4]) = 0x91;

}

//
// Initializes DMA
//
void dma_initialize(void)
{
    dma_regs = (struct dma_regs*)DMA_REGISTERS_BASE_ADDRESS;
    dma_regs->dma_config |= 0x1;
    dma_regs->dma_channel_map_select_0 = ((0x1) | (0x1 << 4)); // Channel 0 UART2 Rx, Channel 1 UART2 Tx
    dma_regs->dma_channel_ctl_base_ptr = (uint32*)0x20002000;
    //dma_regs->dma_channel_priority_set |= (uint32)0x1;
    dma_regs->dma_primary_alt_clear |= (uint32)(1 << 6);
    dma_regs->dma_channel_enable_set |= (uint32)0x1;//enables CH0 only for now (Rx)
    dma_regs->dma_channel_enable_set |= (uint32)(1 << 6);
    dma_regs->dma_channel_req_mask_set |= (uint32)0xFFBF;//(uint32)0xFFBC;//enables requests for only CH0 and CH1 and CH6(SW)

    dma_channel_control_table_init();



}

global void dma_enable(void)
{
    *(dma_regs->dma_channel_ctl_base_ptr + 26UL) |= (uint32)(0x2); //2 for CH0
    dma_regs->dma_channel_enable_set |= (uint32)0x41;//enables CH0 only for now (Rx)
    dma_regs->dma_software_req |= (1 << 6); // sw request for channel 6
    dma_regs->dma_channel_req_mask_set |= (1 << 6); // disables mask - now only does sw
}
