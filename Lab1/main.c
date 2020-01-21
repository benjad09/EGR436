#include "msp.h"
#include "myUART.h"
#include "Timer.h"
#include "App.h"

uint16_t bpm;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop watchdog timer

    InitComms();                                    // Initiate UART communications
    Init_SYSTICK();                                 // Initiate Ststick timer
    InitApp();                                      // Initiate the blinking LED application
    __enable_irq();                                 // Enable interrupts
    __enable_interrupts();

    DebugPrint("hello world\r\n");

    while(1)
    {
        HandleComms();      // Handle UART communications
        HandleDebug();      // Debug UART communications
        HandleApp();        // Handle the blinking LED application
    }
}
