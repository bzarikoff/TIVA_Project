/*
 * gpio.h
 *
 *  Created on: Jan. 18, 2022
 *      Author: braed
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "aaaa.h"

// RCG address
#define GPIO_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS                0x400fe608;
#define GPIO_HIBERNATION_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS    0x400fe614;
#define GPIO_HIGH_PERFORMANCE_BUS_CONTROL_ADDRESS                 0x400FE06c;

// GPIO registers start address (advanced-high performance bus)
#define GPIO_REGS_PORTA                                     0x40058000;
#define GPIO_REGS_PORTB                                     0x40059000;
#define GPIO_REGS_PORTC                                     0x4005A000;
#define GPIO_REGS_PORTD                                     0x4005B000;
#define GPIO_REGS_PORTE                                     0x4005C000;
#define GPIO_REGS_PORTF                                     0x4005D000;

//
// Enumeration of GPIO Ports
//
enum gpio_ports
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    MAX_PORTS,
};

//
// Enumeration of GPIO Pins - max 8 pins per port
//
enum gpio_number
{
    GPIO_0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_4,
    GPIO_5,
    GPIO_6,
    GPIO_7,
};

//
// Enumeration of possible peripheral function numbers
//
enum periph_function
{
    Analog_Function,
    Digital_function1,
    Digital_function2,
    Digital_function3,
    Digital_function4,
    Digital_function5,
    Digital_function6,
    Digital_function7,
    Digital_function8,

};

// Bit Shifts for GPIO function
#define GPIO_PORT_BIT_SHIFT                    13U
#define GPIO_PIN_BIT_SHIFT                     9U
#define GPIO_AFE_BIT_SHIFT                     8U
#define GPIO_PERIPH_FUNCTION_BIT_SHIFT         4U
#define GPIO_PULL_UP_BIT_SHIFT                 3U
#define GPIO_PULL_DOWN_BIT_SHIFT               2U
#define GPIO_DIGITAL_ENABLE_BIT_SHIFT          1U
#define GPIO_DIRECTION_BIT_SHIFT               0U

// Masks
#define GPIO_PORT_MASK                         7U
#define GPIO_PIN_MASK                          15U

//Pullup/Pull down
#define PULLUP       0x2
#define PULLDOWN     0x1
#define NONE         0x0

// GPIO Peripheral functions
#define UART2RX      0x1
#define UART2TX      0x1


//GPIO Functionality
// Direction: 0 = input, 1 = output
#define GPIO_PA0_UART0_RX       (uint16)((PORT_A << GPIO_PORT_BIT_SHIFT) | (GPIO_0 << GPIO_PIN_BIT_SHIFT))
#define GPIO_PA1_UART0_TX       (uint16)((PORT_A << GPIO_PORT_BIT_SHIFT) | (GPIO_1 << GPIO_PIN_BIT_SHIFT))
#define GPIO_PD6_UART2_RX       (uint16)((PORT_D << GPIO_PORT_BIT_SHIFT) | (GPIO_6 << GPIO_PIN_BIT_SHIFT) \
                                    | (1 << GPIO_AFE_BIT_SHIFT) | (UART2RX << GPIO_PERIPH_FUNCTION_BIT_SHIFT))
#define GPIO_PD7_UART2_TX       (uint16)((PORT_D << GPIO_PORT_BIT_SHIFT) | (GPIO_7 << GPIO_PIN_BIT_SHIFT) \
                                    | (1 << GPIO_AFE_BIT_SHIFT) | (UART2TX << GPIO_PERIPH_FUNCTION_BIT_SHIFT))


struct gpio_regs
{
   uint32 gpio_reserved0[255];
   uint32 gpio_data;
   uint32 gpio_dir;
   uint32 gpio_interrupt_sense;
   uint32 gpio_interrupt_both_edges;
   uint32 gpio_interrupt_event;
   uint32 gpio_interrupt_mask;
   uint32 gpio_raw_interrupt_status;
   uint32 gpio_masked_interrupt_status;
   uint32 gpio_interrupt_clear;
   uint32 gpio_alternate_function_select;
   uint32 gpio_reserved1[55];
   uint32 gpio_2mA_drive;
   uint32 gpio_4mA_drive;
   uint32 gpio_8mA_drive;
   uint32 gpio_open_drain_select;
   uint32 gpio_pull_up_select;
   uint32 gpio_pull_down_select;
   uint32 gpio_slew_rate;
   uint32 gpio_digital_enable;
   uint32 gpio_lock;
   uint32 gpio_commit;
   uint32 gpio_analog_mode_select;
   uint32 gpio_port_control;
   uint32 gpio_adc_control;
   uint32 gpio_dma_control;
   uint32 gpio_reserved2[678];
   uint32 gpio_periph_id_4;
   uint32 gpio_periph_id_5;
   uint32 gpio_periph_id_6;
   uint32 gpio_periph_id_7;
   uint32 gpio_periph_id_0;
   uint32 gpio_periph_id_1;
   uint32 gpio_periph_id_2;
   uint32 gpio_periph_id_3;
   uint32 gpio_prime_cell_id_0;
   uint32 gpio_prime_cell_id_1;
   uint32 gpio_prime_cell_id_2;
   uint32 gpio_prime_cell_id_3;
};


extern void gpio_initialize(const uint16 gpio_function_array[], uint32 array_size);
extern void gpio_clock_enable(uint32 ports_to_enable);
extern void gpio_configure(uint16 gpio_function);


#endif /* GPIO_H_ */
