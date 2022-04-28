/*
 * dma.h
 *
 *  Created on: Feb. 9, 2022
 *      Author: braed
 */

#ifndef DMA_H_
#define DMA_H_

#include "aaaa.h"

#define DMA_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS        0x400FE60C
#define DMA_REGISTERS_BASE_ADDRESS                       0x400FF000

struct dma_channel_control
{
    uint32 dma_channel_source_address_pointer;
    uint32 dma_channel_destination_address_pointer;
    uint32 dma_channel_control_word;
};

struct dma_regs
{
    uint32 dma_status;
    uint32 dma_config;
    uint32 *dma_channel_ctl_base_ptr;
    uint32 *dma_alt_channel_ctl_base_ptr;
    uint32 dma_wait_status;
    uint32 dma_software_req;
    uint32 dma_channel_useburst_set;
    uint32 dma_channel_useburst_clear;
    uint32 dma_channel_req_mask_set;
    uint32 dma_channel_req_mask_clear;
    uint32 dma_channel_enable_set;
    uint32 dma_channel_enable_clear;
    uint32 dma_primary_alt_set;
    uint32 dma_primary_alt_clear;
    uint32 dma_channel_priority_set;
    uint32 dma_channel_priority_clear;
    uint32 dma_reserved1[3];
    uint32 dma_bus_error_clear;
    uint32 dma_reserved2[300];
    uint32 dma_channel_assign;
    uint32 dma_channel_intr_status;
    uint32 dma_reserved3[2];
    uint32 dma_channel_map_select_0;
    uint32 dma_channel_map_select_1;
    uint32 dma_channel_map_select_2;
    uint32 dma_channel_map_select_3;
    uint32 dma_reserved4[687];
    uint32 dma_periph_id_4;
    uint32 dma_periph_id_0;
    uint32 dma_periph_id_1;
    uint32 dma_periph_id_2;
    uint32 dma_periph_id_3;
    uint32 dma_prime_cell_id_0;
    uint32 dma_prime_cell_id_1;
    uint32 dma_prime_cell_id_2;
    uint32 dma_prime_cell_id_3;
};

extern void dma_clock_enable(void);
extern void dma_initialize(void);
extern void dma_enable(void);

#endif /* DMA_H_ */
