#include "msp.h"
#include "UART.h"
#include "mySPI.h"
#include "Timer.h"
#include "App.h"
#include "FRAM.h"


/**
 * main.c
 */
uint16_t bpm;


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    InitComms();
    Init_SYSTICK();
    InitApp();
    InitSPI();
    InitFRAM();
    __enable_irq();//enable interrupts
    __enable_interrupts();
    DebugPrint("VERSION:1\r\n");

    while(1)
    {
        HandleComms();
        HandleDebug();
        HandleApp();
        HandleSPI();
        HandleFRAM();
    }
}
