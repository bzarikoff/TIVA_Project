/*
 * cpu.h
 *
 *  Created on: Jan. 31, 2022
 *      Author: braed
 */

#ifndef CPU_H_
#define CPU_H_

#include "aaaa.h"


#define HZ_PER_MHZ              1000000UL
#define SYS_CLOCK_MHZ           16UL
#define SYS_CLOCK_HZ            (SYS_CLOCK_MHZ * HZ_PER_MHZ)

#define SYSTICK_CONTROL_AND_STATUS             0xE000E010
#define SYSTICK_RELOAD_VAL                     0xE000E014
#define SYSTICK_CURRENT_VAL                    0xE000E018


//
// True if the current time has passed the expiry time; i.e., you should do what you were
// waiting for. (At __ minutes past the expiry time, it becomes
// false again.)
//
#define CPU_TIME_IS_AFTER(current, expiry)  (((int32)(expiry) - (int32)(current)) > 0)

//
// Return a timestamp for the current time.  Uses lower 24 bits. 0x00FFFFFF to zero is one period
// at system clock period
//
//#define cpu_microseconds() ((int32)REGISTER_32(0xE000E018) )//& 0x00FFFFFF)

extern void timer_init(void);
extern int32 cpu_timer_value(void);
extern int32 cpu_expiry_time(int32 cpu_timer, uint32 period);

#endif /* CPU_H_ */
