/*
 * application.c
 *
 *  Created on: April. 13, 2022
 *      Author: braed
 */

#include "serial/serial.h"
#include "aaaa.h"
#include "gpio/gpio.h"

local uint32 test_value;

//
// Do something useful with value sent from dma
//
void test_function_store_application(uint32 value_received)
{
    test_value = value_received;

}
