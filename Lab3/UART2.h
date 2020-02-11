/*
 * UART.h
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#ifndef UART_H2_
#define UART_H2_

/**********************************************************
 *
 *  File: uart.h
 *  Description: Header file with UART2 driver definitions & prototypes
 *
 *  Phase 1 Engineering, LLC
 *  2019
 *
 **********************************************************/


#include "msp.h"

extern void InitComms2( void );
extern void HandleComms2( void );
extern void sendString2( char *str, uint32_t n);
uint8_t reciveString2(char *str);

extern void DebugPrint2( const char *str );
extern void PrintIs2( const char *str, int16_t val );
extern void PrintI2( const char *str, int16_t val );
extern void PrintUs2( const char *str, uint16_t val );
extern void PrintU2( const char *str, uint16_t val );
extern void PrintDEC2(const char *str, uint32_t val);
extern void PrintHEX2( const char *str, uint16_t val );
extern void PrintHEXL2( const char *str, uint16_t val );

void HandleDebug2( void );
extern uint8_t stringcom2pare2(char *str1 ,char *str2,uint8_t length);



#define TX_BUF_N2          1024
#define RX_BUF_N2          256
#define MAX_STRING2        256
#define MAX_PARSE_BYTES2   256


#define STORE2 "STORE"
#define STORE_L2 5

#define DIRECT2 "DIR"
#define DIRECT_L2 3

#define SIZE2 "MEM"
#define SIZE_L2 3

#define DELETE2 "DELETE"
#define DELETE_L2 6

#define READ2 "READ"
#define READ_L2 4

#define CLEAR2 "CLEAR"
#define CLEAR_L2 5







#endif /* UART_H2_ */
