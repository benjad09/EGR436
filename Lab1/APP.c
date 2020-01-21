#include "APP.h"
#include "msp.h"
#include "Timer.h"

uint16_t bpm;

void InitApp(void)
{
    bpm = 60;               // Set initial beats per minute for LED
    P1->SEL1 &= ~BIT0;      // Configure on-board LED as output and drive LOW
    P1->SEL0 &= ~BIT0;
    P1->DIR |= BIT0;
    P1->OUT |= BIT0;
}

void HandleApp(void)
{
    static uint64_t blinky = 0,delay=0;
    if(GetUpTime()>blinky)      // After ON time is done...
    {
        blinky = GetUpTime()+ 60000/bpm;        // Update LED ON time
        delay = GetUpTime()+60000/(10*bpm);     // Update LED OFF time
        P1->OUT |= BIT0;                        // Turn LED ON
    }
    if(GetUpTime()>delay)       // After OFF time is done...
    {
        P1->OUT &= ~BIT0;                       // Turn LED OFF
    }
}

void setbpm(uint16_t val)
{
    bpm = val;              // Set beats per minute for LED
}
