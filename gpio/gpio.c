/*
 * gpio.c
 *
 *  Created on: Jan. 18, 2022
 *      Author: braed
 */

#include "gpio/gpio.h"
#include "aaaa.h"

static struct gpio_regs *gpio_regs[(int16)MAX_PORTS];

//
// Enables the run time clock for a gpio port
// Input is a bitfield representing each port
//
global void gpio_clock_enable(uint32 ports_to_enable)
{
    if(ports_to_enable > 0b1111111)
        return;

    uint32 *gpio_enable = (uint32*)GPIO_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS;
    uint32 *hibernation_clock_enable = (uint32*)GPIO_HIBERNATION_RUN_MODE_CLOCK_GATING_CONTROL_ADDRESS;

    *gpio_enable |= (( ports_to_enable >> (int32)PORT_A) && 0b1) << (int32)PORT_A;
    *gpio_enable |= (( ports_to_enable >> (int32)PORT_B) && 0b1) << (int32)PORT_B;
    *gpio_enable |= (( ports_to_enable >> (int32)PORT_C) && 0b1) << (int32)PORT_C;
    *gpio_enable |= (( ports_to_enable >> (int32)PORT_D) && 0b1) << (int32)PORT_D;
    *gpio_enable |= (( ports_to_enable >> (int32)PORT_E) && 0b1) << (int32)PORT_E;
    *gpio_enable |= (( ports_to_enable >> (int32)PORT_F) && 0b1) << (int32)PORT_F;

    // clock provided to modules in hibernation mode
    *hibernation_clock_enable |= 0x1;

}

//
// Configure special pins
//
void gpio_special_pin_configure(uint16 gpio_function, uint16 port, uint16 pin_number, uint16 alternate_function_select, uint16 peripheral_function, uint16 pullup, uint16 pulldown, uint16 digital_enable, uint16 direction )
{
    if((port == (uint16)PORT_D) && (pin_number == 7))
    {
        // For PD7 GPIOLOCK needs to be unlocked then GPIO_CR needs to be set before AFE, PCT reg can
        // be programmed
        gpio_regs[(int16)PORT_D]->gpio_lock = 0x4C4F434B;
        gpio_regs[(int16)PORT_D]->gpio_commit |= (1 << pin_number);
        gpio_regs[(int16)port]->gpio_alternate_function_select |= (alternate_function_select << pin_number);
        gpio_regs[(int16)port]->gpio_port_control |= (peripheral_function << pin_number*4);
        gpio_regs[(int16)port]->gpio_pull_up_select |= (pullup << pin_number);
        gpio_regs[(int16)port]->gpio_pull_down_select |= (pulldown << pin_number);
        gpio_regs[(int16)port]->gpio_digital_enable |= (digital_enable << pin_number);
        gpio_regs[(int16)port]->gpio_dir |= (direction << pin_number);
    }
    else
        return;
}

//
// Configures the given pin with input functionality
// Note that from datasheet: PA[1:0], PA[5:2], PB[3:2], PC[3:0], PD[7], PF[0]
// have difference default states on reset than the other pins and will need to manually configured
//
global void gpio_configure(uint16 gpio_function)
{
    uint16 port = (gpio_function >> GPIO_PORT_BIT_SHIFT) & GPIO_PORT_MASK;
    uint16 pin_number = (gpio_function >> GPIO_PIN_BIT_SHIFT) & GPIO_PIN_MASK;
    uint16 alternate_function_select = (gpio_function >> GPIO_AFE_BIT_SHIFT) & 0x1;
    uint16 peripheral_function = (gpio_function >> GPIO_PERIPH_FUNCTION_BIT_SHIFT) & 0x1;
    uint16 pullup = (gpio_function >> GPIO_PULL_UP_BIT_SHIFT) & 0x1;
    uint16 pulldown = (gpio_function >> GPIO_PULL_DOWN_BIT_SHIFT) & 0x1;
    uint16 digital_enable = (gpio_function >> GPIO_DIGITAL_ENABLE_BIT_SHIFT) & 0x1;
    uint16 direction = (gpio_function >> GPIO_DIRECTION_BIT_SHIFT) & 0x1;

    uint16 special_gpio = ((port == (uint16)PORT_A) && (pin_number <= 5));
    special_gpio |= ((port == (uint16)PORT_B) && ((pin_number == 3) || (pin_number ==2)));
    special_gpio |= ((port == (uint16)PORT_C) && (pin_number <= 3));
    special_gpio |= ((port == (uint16)PORT_D) && (pin_number == 7));
    special_gpio |= ((port == (uint16)PORT_F) && (pin_number == 0));

    if(special_gpio)
        gpio_special_pin_configure(gpio_function, port, pin_number, alternate_function_select, peripheral_function, pullup, pulldown, digital_enable, direction);
    else
    {
        gpio_regs[(int16)port]->gpio_alternate_function_select |= (alternate_function_select << pin_number);
        gpio_regs[(int16)port]->gpio_port_control |= (peripheral_function << pin_number*4);
        gpio_regs[(int16)port]->gpio_pull_up_select |= (pullup << pin_number);
        gpio_regs[(int16)port]->gpio_pull_down_select |= (pulldown << pin_number);
        gpio_regs[(int16)port]->gpio_digital_enable |= (digital_enable << pin_number);
        gpio_regs[(int16)port]->gpio_dir |= (direction << pin_number);
    }
}

//
// Configures each gpio pin
//
global void gpio_initialize(const uint16 gpio_function_array[], uint32 array_size)
{
    if(gpio_function_array == NULL)
        return; //replace with exit crash

    // access all gpio ports through high performance bus
    uint32 *bus = (uint32*)GPIO_HIGH_PERFORMANCE_BUS_CONTROL_ADDRESS;
    *bus |= 0b1111111;

    gpio_regs[(int32)PORT_A] = (struct gpio_regs *)GPIO_REGS_PORTA;
    gpio_regs[(int32)PORT_B] = (struct gpio_regs *)GPIO_REGS_PORTB;
    gpio_regs[(int32)PORT_C] = (struct gpio_regs *)GPIO_REGS_PORTC;
    gpio_regs[(int32)PORT_D] = (struct gpio_regs *)GPIO_REGS_PORTD;
    gpio_regs[(int32)PORT_E] = (struct gpio_regs *)GPIO_REGS_PORTE;
    gpio_regs[(int32)PORT_F] = (struct gpio_regs *)GPIO_REGS_PORTF;

    // unlocks writes to the gpio_commit register
    gpio_regs[(int32)PORT_A]->gpio_lock = 0x4c4f434b;
    gpio_regs[(int32)PORT_B]->gpio_lock = 0x4c4f434b;
    gpio_regs[(int32)PORT_C]->gpio_lock = 0x4c4f434b;
    gpio_regs[(int32)PORT_D]->gpio_lock = 0x4c4f434b;
    gpio_regs[(int32)PORT_E]->gpio_lock = 0x4c4f434b;
    gpio_regs[(int32)PORT_F]->gpio_lock = 0x4c4f434b;

    uint16 index;
    for(index = 0; index < array_size; ++index)
        gpio_configure(gpio_function_array[index]);
}

