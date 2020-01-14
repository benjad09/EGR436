/*
 * Timer.c
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#include "msp.h"

uint64_t uptime;

void Init_SYSTICK(void)
{
    SysTick->CTRL = 0;      // Configure the Systick timer to generate interrupts every millisecond
    SysTick->LOAD = 3000;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;
    uptime = 0;
}

void SysTick_Handler(void)
{
    uptime++;               // Increment number of milliseconds
}

uint64_t GetUpTime(void)
{
 uint64_t temp1,temp2;
 do
 {
     temp1 = uptime;        // Retrieve the current time (with error checking)
     temp2 = uptime;
 }while(temp1 != temp2);    // Re-retrieve the time if an interrupt occurred mid-retrieval
 return(temp1);
}
