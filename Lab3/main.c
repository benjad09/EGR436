#include "msp.h"
#include "UART.h"
#include "UART2.h"
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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop watchdog timer

    InitComms();                    // Initializations
    InitComms2();
    Init_SYSTICK();
    InitApp();
    InitSPI();
    InitFRAM();
    __enable_irq();
    __enable_interrupts();
    DebugPrint("VERSION:1\r\n");
    DebugPrint2("AT+NAMEWARIOWARE");

    while(1)
    {
        HandleComms();              // Call various modules
        HandleComms2();
        HandleDebug();
        HandleDebug2();
        HandleApp();
        HandleSPI();
        HandleFRAM();
    }
}
