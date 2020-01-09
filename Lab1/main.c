#include "msp.h"
#include "UART.h"
#include "Timer.h"
#include "App.h"


/**
 * main.c
 */
uint16_t bpm;


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	InitComms();
	Init_SYSTICK();
	InitApp();
    __enable_irq();//enable interrupts
    __enable_interrupts();

    DebugPrint("hello world\r\n");


	while(1)
	{
	    HandleComms();
	    HandleDebug();
	    HandleApp();
	}
}
