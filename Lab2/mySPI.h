/*
 * mySPI.h
 *
 *  Created on: Jan 14, 2020
 *      Author: bendr
 */

#ifndef MYSPI_H_
#define MYSPI_H_


/**********************************************************
 *
 *  File: uart.h
 *  Description: Header file with UART2 driver definitions & prototypes
 *
 *  2019
 *
 **********************************************************/


#include "msp.h"

extern void InitSPI( void );
extern void HandleSPI( void );


void writedata(uint16_t Address,uint8_t data);
void readdata(uint16_t Address);
uint8_t peakSPI(void);
uint8_t reciveSPI(char *str);
uint8_t SPIbusy(void);







#define TX_BUF_N_SPI          256
#define RX_BUF_N_SPI          256
#define MAX_STRING_SPI        256
#define MAX_PARSE_BYTES_SPI   256




//#define PRINTTXRX





#endif /* MYSPI_H_ */
