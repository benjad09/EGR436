/*
 * UART.c
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#include "mySPI.h"
#include "UART.h"
#include "APP.h"
#include "Timer.h"
#include "msp.h"

// Structure for SPI data processing
static struct
{
  struct                // SPI receive buffer
  {
    uint32_t head;
    uint32_t tail;
    char buf[RX_BUF_N];
  } rx;
  struct                // SPI transmit buffer
  {
    uint32_t head;
    uint32_t tail;
    uint8_t endofcommand[TX_BUF_N_SPI];
    uint8_t readdata[TX_BUF_N_SPI];
    char buf[TX_BUF_N_SPI];
  } tx;
  struct                // SPI parsing
  {
    uint8_t nBytes;
    char buf[MAX_PARSE_BYTES];
  } parse;
} SPI;

/////////////////////////////////   Functions  ////////////////////////////////
static void UpdateRxTxSPI( void );
static void sendSPI( char *str, uint32_t n,uint8_t read);

char status[5] = {0x9f,0x00,0x00,0x00,0x00};
char read[2] = {0x05,0x00};
char enable = 0x06;

// Initiate SPI communications
void InitSPI( void )
{
    SPI.rx.head = 0;                // Initialize buffer heads and tails
    SPI.rx.tail = 0;
    SPI.tx.head = 0;
    SPI.tx.tail = 0;
    SPI.parse.nBytes = 1;
    uint16_t i = 0;
    for(i=0;i<TX_BUF_N_SPI;i++)     // Data cleared to 0
    {
        SPI.tx.endofcommand[i] = 0;
        SPI.tx.readdata[i] = 0;
    }

    P5->SEL1 &= ~BIT2;              // GPIO pin for CS, set to output, set HIGH
    P5->SEL0 &= ~BIT2;
    P5->DIR |= BIT2;
    P5->OUT |= BIT2;

    P3->SEL0 |=  (BIT5 | BIT6 | BIT7); // P1.2 and P1.3 are EUSCI_A0 RX
    P3->SEL1 &= ~(BIT5 | BIT6 | BIT7); // and TX respectively.


    // Baud Rate Configuration
    // 3000000/(16*9600) = 19.531  (3 MHz at 9600 bps is fast enough to turn on over sampling (UCOS = /16))
    //UCCKPH = 1;   //(0ver sampling, /16 turned on)
    //UCBR  = 19 ;  //(Whole portion of the divide)
    //UCBRF = 8.5;  // .53125 * 16 (0x0A) (Remainder of the divide)
    //UCBRS = 0xAA; //remainder=0.531 (look up table 22-4)
    //EUSCI_B2->CTLW0 = 0x0D80;
    //EUSCI_B2->CTLW0 = 0x0B80;
    //EUSCI_B2->CTLW0 = 0xAD83;
    EUSCI_B2->CTLW0 |= 0b1010100110000000;
    EUSCI_B2->CTLW0 &= ~0b0101011001000011;

    //EUSCI_A0->BRW = 26;
    EUSCI_B2->BRW = 0;  // UCBR Value from above

    //EUSCI_B2->CTLW0 &= ~BIT0;
    EUSCI_B2->CTLW0 &= ~0x0001;     // Enable EUSCI
    EUSCI_B2->IFG &= ~BIT0;         // Clear interrupt
    EUSCI_B2->IE |= BIT0;           // Enable interrupt
    NVIC_EnableIRQ(EUSCIB2_IRQn);   // Enable handler in NVIC

    //DebugPrint("SPI 'online'\r\n");
    P5->OUT |= BIT2;
    sendSPI(status,5,0);            // Read device ID (FRAM opcode)
    sendSPI(&enable,1,0);           // Set write enable latch (FRAM opcode)
    sendSPI(read,2,0);              // Read status register (FRAM opcode)
}

//Check status of transmit and receive UARTs
void HandleSPI( void )
{
    UpdateRxTxSPI();
}

// Parse received data; handle SPI buffer appropriately
void ParseRxData( void )
{
    char newByte = 'a';

    while (SPI.rx.head != SPI.rx.tail)
    {
         newByte = SPI.rx.buf[SPI.rx.tail++];

        if (SPI.rx.tail > RX_BUF_N)
        {
            SPI.rx.tail = 0;
        }
        if ((SPI.parse.nBytes < MAX_PARSE_BYTES))
        {
            SPI.parse.buf[SPI.parse.nBytes++] = newByte;
        }
        else
        {   //Buffer is full or got a newline/CR: check for meaningful messages
            SPI.parse.nBytes = 0;
        }
    }
}

// Write data to SPI
void writedata(uint16_t Address,uint8_t data)
{
    char command[4];
    if (Address>=4096)
    {
        Address=4096;
    }
    command[0] = 0x02;              // Write memory code (FRAM opcode)
    command[1] = (Address>>8)&0xff;
    command[2] = Address&0xff;
    command[3] = data;
    sendSPI(&enable,1,0);           // Set write enable latch (FRAM opcode)
    sendSPI(command,4,0);           // Send data
}

// Read data from SPI
void readdata(uint16_t Address)
{
    char command[4];
    if (Address>=4096)
    {
        Address=4096;
    }
    command[0] = 0x03;              // Read memory code (FRAM opcode)
    command[1] = (Address>>8)&0xff;
    command[2] = Address&0xff;
    command[3] = 0x00;
    sendSPI(command,4,1);           // Read data
}

// Receive a string from SPI
uint8_t reciveSPI(char *str){
    int j=0;
    for(j=0;j<SPI.parse.nBytes;j++)
    {
        str[j] = SPI.parse.buf[j];
    }
    uint8_t temp = SPI.parse.nBytes;
    SPI.parse.nBytes = 0;
    return temp;
}

// SPI interrupt handler
void EUSCIB2_IRQHandler(void)
{
            do
            {  // Store the new piece of data at the present location in the buffer
                SPI.rx.buf[SPI.rx.head] = EUSCI_B2->RXBUF;
#ifdef PRINTTXRX
                PrintHEX("SPI<-",SPI.rx.buf[SPI.rx.head]);
                DebugPrint("\r\n");
#endif
                if(SPI.tx.readdata[SPI.tx.tail])
                   {
                      SPI.tx.readdata[SPI.tx.tail] = 0;
                      PrintUs("r",SPI.rx.buf[SPI.rx.head]);
                      DebugPrint("\r\n");
                   }
                if (++SPI.rx.head >= RX_BUF_N)
                {
                    SPI.rx.head = 0;
                }
            }while(EUSCI_B2->IFG & BIT0);
}

// Queue up a string "str" of n characters to transmit: send from main loop
static void sendSPI(char *str, uint32_t n,uint8_t read)
{
  uint32_t j=0;
  for (j=0; j<n; j++)
  {
    SPI.tx.buf[SPI.tx.head++] = str[j];
    if (SPI.tx.head >= TX_BUF_N_SPI)
    {
      SPI.tx.head = 0;
    }
  }
  if(SPI.tx.head == 0)
  {
      SPI.tx.endofcommand[TX_BUF_N_SPI-1]=1;
      if (read)
      {
          SPI.tx.readdata[TX_BUF_N_SPI-1] = 1;
      }
  }
  else
  {
      SPI.tx.endofcommand[SPI.tx.head-1]=1;
      if(read)
      {
          SPI.tx.readdata[SPI.tx.head-1] = 1;
      }
  }
}

static void UpdateRxTxSPI( void )
{
    uint64_t commandbreak = 0;

    if(SPI.tx.head != SPI.tx.tail)
    {
        if (GetUpTime()>commandbreak)
        {
        P5->OUT &= ~BIT2;                           // Turn CS pin OFF
            if((EUSCI_B2->IFG & BIT1))
            {
#ifdef PRINTTXRX
                PrintHEX("SPI->",SPI.tx.buf[SPI.tx.tail]);
                DebugPrint("\r\n");
#endif

                EUSCI_B2->TXBUF = SPI.tx.buf[SPI.tx.tail];
                if(SPI.tx.endofcommand[SPI.tx.tail])
                {
                   SPI.tx.endofcommand[SPI.tx.tail] = 0;
                   P5->OUT |= BIT2;                 // Turn CS pin ON
                }
                if (++SPI.tx.tail >= TX_BUF_N_SPI)
                {
                   SPI.tx.tail = 0;
                }
            }
        }
    }
    else{
        P5->OUT |= BIT2;          // Turn CS pin ON
        }
}
