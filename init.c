/*
 * init.c
 *
 *  Created on: Jan. 18, 2022
 *      Author: braed
 */

#include "serial/serial.h"
#include "aaaa.h"
#include "gpio/gpio.h"

local const uint16 gpio_array[] =
{
 // Port A
 GPIO_PA0_UART0_RX,
 GPIO_PA1_UART0_TX,
 //GPIO_PA2 is unused
 //GPIO_PA3 is unused
 //GPIO_PA4 is unused
 //GPIO_PA5 is unused
 //GPIO_PA6 is unused
 //GPIO_PA7 is unused

 //Port B
 //GPIO_PB0 is unused
 //GPIO_PB1 is unused
 //GPIO_PB2 is unused
 //GPIO_PB3 is unused
 //GPIO_PB4 is unused
 //GPIO_PB5 is unused
 //GPIO_PB6 is unused
 //GPIO_PB7 is unused

 //Port C
 //GPIO_PC0 is unused
 //GPIO_PC1 is unused
 //GPIO_PC2 is unused
 //GPIO_PC3 is unused
 //GPIO_PC4 is unused
 //GPIO_PC5 is unused
 //GPIO_PC6 is unused
 //GPIO_PC7 is unused

 //Port D
 //GPIO_PD0 is unused
 //GPIO_PD1 is unused
 //GPIO_PD2 is unused
 //GPIO_PD3 is unused
 //GPIO_PD4 is unused
 //GPIO_PD5 is unused
 GPIO_PD6_UART2_RX,
 GPIO_PD7_UART2_TX,

 //Port E
 //GPIO_PE0 is unused
 //GPIO_PE1 is unused
 //GPIO_PE2 is unused
 //GPIO_PE3 is unused
 //GPIO_PE4 is unused
 //GPIO_PE5 is unused

 //Port F
 //GPIO_PF0 is unused
 //GPIO_PF1 is unused
 //GPIO_PF2 is unused
 //GPIO_PF3 is unused
 //GPIO_PF4 is unused
};

void uart_initialize(void)
{
    uart_clock_enable(UART_TWO);
    uart_clock_enable(UART_ZERO);
    uart_configure(UART_TWO);
    uart0_configure_for_test(UART_ZERO);
}

//
// Initialization routine of gpios
//
void gpio_init(void)
{
    gpio_clock_enable(0b1111111);
    gpio_initialize( gpio_array, sizeof(gpio_array) / sizeof(gpio_array[0]) );
}

//
// Initialization routine for DMA
//
void dma_init(void)
{
    dma_clock_enable();
    dma_initialize();
}

//
// Initialize peripherals
//
global void initialization_routine()
{
    gpio_init();
   // dma_init();
    uart_initialize();
}
