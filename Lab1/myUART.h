/*
 * UART.h
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#ifndef MYUART_H_
#define MYUART_H_

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



#define TX_BUF_N          256
#define RX_BUF_N          64
#define MAX_STRING        64
#define MAX_PARSE_BYTES   64







#endif /* MYUART_H_ */
