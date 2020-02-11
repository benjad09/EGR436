/*
 * Timer.c
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#include "msp.h"

uint64_t uptime;

void Init_SYSTICK(void)     // Configure SysTick timer
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 3000;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;
    uptime = 0;
}

void SysTick_Handler(void)  // SysTick timer interrupt handler
{
    uptime++;                       // Increment time variable
}

uint64_t GetUpTime(void)    // Retreive time variable
{
 uint64_t temp1,temp2;
 do
 {
     temp1 = uptime;
     temp2 = uptime;
 }while(temp1 != temp2);
 return(temp1);
}

