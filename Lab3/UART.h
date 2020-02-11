/*
 * UART.h
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#ifndef UART_H_
#define UART_H_

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

extern void InitComms( void );
extern void HandleComms( void );
extern void sendString( char *str, uint32_t n);
uint8_t reciveString(char *str);

extern void DebugPrint( const char *str );
extern void PrintIs( const char *str, int16_t val );
extern void PrintI( const char *str, int16_t val );
extern void PrintUs( const char *str, uint16_t val );
extern void PrintU( const char *str, uint16_t val );
extern void PrintDEC(const char *str, uint32_t val);
extern void PrintHEX( const char *str, uint16_t val );
extern void PrintHEXL( const char *str, uint16_t val );

void HandleDebug( void );
extern uint8_t stringcompare(char *str1 ,char *str2,uint8_t length);



#define TX_BUF_N          1024
#define RX_BUF_N          256
#define MAX_STRING        256
#define MAX_PARSE_BYTES   256


#define STORE "STORE"
#define STORE_L 5

#define DIRECT "DIR"
#define DIRECT_L 3

#define SIZE "MEM"
#define SIZE_L 3

#define DELETE "DELETE"
#define DELETE_L 6

#define READ "READ"
#define READ_L 4

#define CLEAR "CLEAR"
#define CLEAR_L 5







#endif /* UART_H_ */
