/*
 * APP.c
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#include "APP.h"
#include "msp.h"
#include "Timer.h"

uint16_t bpm;


void InitApp(void)
{
    bpm = 60;
    P1->SEL1 &= ~BIT0;
    P1->SEL0 &= ~BIT0;
    P1->DIR |= BIT0;
    P1->OUT |= BIT0;

}

void HandleApp(void)
{
    static uint64_t blinky = 0,delay=0;
    if(GetUpTime()>blinky)
    {
        blinky = GetUpTime()+ 60000/bpm;
        //delay = GetUpTime()+100;
        delay = GetUpTime()+60000/(10*bpm);
        P1->OUT |= BIT0;
    }
    if(GetUpTime()>delay)
    {
        P1->OUT &= ~BIT0;
    }
}

void setbpm(uint16_t val)
{
    bpm = val;
}


