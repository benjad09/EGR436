/*
 * UART.c
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */


#include "UART2.h"
#include "mySPI.h"
#include "APP.h"
#include "msp.h"
#include "FRAM.h"
#include "Timer.h"

static struct
{
  struct                    // Receive buffer management
  {
    uint32_t head;
    uint32_t tail;
    char buf[RX_BUF_N2];
  } rx;
  struct                    // Transmit buffer management
  {
    uint32_t head;
    uint32_t tail;
    char buf[TX_BUF_N2];
  } tx;
  struct                    // Data parsing management
  {
    uint8_t nBytes;
    char buf[MAX_PARSE_BYTES2];
  } parse;
} com2;

/////////////////////////////////   Functions  ////////////////////////////////
static void UpdateRxTx2( void );
static void ParseRxData2( void );

void InitComms2( void )          // Initiate UART com2munications
{
    com2.rx.head = 0;
    com2.rx.tail = 0;
    com2.tx.head = 0;
    com2.tx.tail = 0;
    com2.parse.nBytes = 1;

    P3->SEL0 |=  (BIT2 | BIT3); // P1.2 and P1.3 are EUSCI_A2 RX
    P3->SEL1 &= ~(BIT2 | BIT3); // and TX respectively.

    EUSCI_A2->CTLW0 = 0x00C1;
    EUSCI_A2->BRW = 26;
    EUSCI_A2->MCTLW &= ~0xFFF1;
    EUSCI_A2->MCTLW = 0;

    EUSCI_A2->CTLW0 &= ~BIT0;  // Enable EUSCI
    EUSCI_A2->IFG &= ~BIT0;    // Clear interrupt
    EUSCI_A2->IE |= BIT0;      // Enable interrupt
    NVIC_EnableIRQ(EUSCIA2_IRQn);
}

void HandleComms2( void )       // Handle UART com2munication
{
  UpdateRxTx2();     //Check status of transmit and receive UART
  ParseRxData2();    //See if we received any messages
}

void ParseRxData2( void )       // Parse received data
{
    char newByte = 'a';

    while (com2.rx.head != com2.rx.tail)
    {
         newByte = com2.rx.buf[com2.rx.tail++];

        if (com2.rx.tail > RX_BUF_N2)
        {
            com2.rx.tail = 0;
        }
        if ((com2.parse.nBytes < MAX_PARSE_BYTES2))
        {
            com2.parse.buf[com2.parse.nBytes++] = newByte;
        }
        else
        { //Buffer is full or got a newline/CR: check for meaningful messages
            com2.parse.nBytes = 0;
        }
    }
}
                        // Receive a string from UART
uint8_t reciveString2(char *str){
    int j=0;
    for(j=0;j<com2.parse.nBytes;j++)
    {
        str[j] = com2.parse.buf[j];
    }
    uint8_t temp = com2.parse.nBytes;
    com2.parse.nBytes = 0;
    return temp;
}
                        // UART interrupt handler
void EUSCIA2_IRQHandler(void)
{
            do
            {
                com2.rx.buf[com2.rx.head] = EUSCI_A2->RXBUF; // store the new piece of data at the present location in the buffer
                com2.rx.head++;
                if (com2.rx.head >= RX_BUF_N2)
                {
                    com2.rx.head = 0;
                }
            }while(EUSCI_A2->IFG & BIT0);
}
                    //Queue up a string "str" of n characters to transmit: send from main loop
void sendString2( char *str, uint32_t n)
{
    uint32_t j=0;
  for (j=0; j<n; j++)
  {
    com2.tx.buf[com2.tx.head++] = str[j];
    if (com2.tx.head >= TX_BUF_N2)
    {
      com2.tx.head = 0;
    }
  }
}
                    // Update receive/transmit buffer management
static void UpdateRxTx2( void )
{
    if(com2.tx.head != com2.tx.tail)
    {
        if((EUSCI_A2->IFG & BIT1))
        {
            EUSCI_A2->TXBUF = com2.tx.buf[com2.tx.tail];

            if (++com2.tx.tail >= TX_BUF_N2)
            {
               com2.tx.tail = 0;
            }
        }
    }
}
                    // Data conversion function
static void usitoa4( uint16_t val, char *str )
{
    uint16_t pow;
    int16_t i;
    uint16_t firstVal = 0;

    if (val > 9999)
    {
        for (i=0; i<4; i++)
        {
            str[i] = 'x';   //Out of range
        }
        return;
    }
    //Load padding characters into output
    for (i=0; i<4; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<4; i++)
    {
        switch(i)
        {
            case 0:
                pow=1000;
                break;
            case 1:
                pow=100;
                break;
            case 2:
                pow=10;
                break;
            case 3:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
    }
}

//Function: usitoa5
//Inputs:
//  val:        Value to convert, range 0-65535
//  *str:       Location to save resulting string: SIZE WILL BE 5 BYTES, 0-PADDED
//Outputs: none
//
//Description: "Unsigned Int to ASCII - 5-character"
//Converts an unsigned integer, range 0-65535, to an ascii number string with
//'0'-padding to 5 characters
//Stack load: 6 bytes
static void usitoa5( uint16_t val, char *str )
{
    uint16_t pow;
    uint16_t i;
    uint16_t firstVal = 0;

    //Load padding characters into output
    for (i=0; i<5; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<5; i++)
    {
        switch(i)
        {
            case 0:
                pow=10000;
                break;
            case 1:
                pow=1000;
                break;
            case 2:
                pow=100;
                break;
            case 3:
                pow=10;
                break;
            case 4:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
    }
}
                // Data conversion function
static void usitoa2h( uint16_t val, char *str )
{
    uint16_t pow;
    uint16_t i;
    uint16_t firstVal = 0;

    //Load padding characters into output
    for (i=0; i<2; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<2; i++)
    {
        switch(i)
        {
            case 0:
                pow=16;
                break;
            case 1:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
                if (str[i]>57)
                {
                    str[i] += 39;
                }
    }
}
                    // Data conversion function
static void usitoa3h( uint16_t val, char *str )
{
    uint16_t pow;
    uint16_t i;
    uint16_t firstVal = 0;

    //Load padding characters into output
    for (i=0; i<3; i++)
    {
        str[i] = '0';
    }

    for (i=0; i<3; i++)
    {
        switch(i)
        {
            case 0:
                pow=256;
                break;
            case 1:
                pow=16;
                break;
            case 2:
                pow=1;
                break;
        }
        if (val >= pow)
        {
            firstVal = 1;
        }
        if (firstVal !=0)
        {
            str[i] = '0';
        }
        while( val >= pow )
        {
            val -= pow;
            str[i]++;
        }
                if (str[i]>57)
                {
                    str[i] += 39;
                }
    }
}
//Function: DebugPrint
//Inputs: char *str - pointer to data to send; must be null-terminated
//Outputs: none
//
//Description:
//Prints a string out the debug UART.  Must be null-terminated.
//Example: DebugPrint( "Hello world!\r\n" );
void DebugPrint2( const char *str )
{
  uint32_t i;
  for (i=0; i<MAX_STRING2; i++)
  {
    if (0 == str[i])
    {
        break;
    }
  }
  if (i>0)
  {
    sendString2( (char*)str, i );
  }
}

//Function: PrintU
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          UINT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and number out the debug UART. String must be null-terminated.
//Example: PrintU( "Value=", 349 ); results in "Value=00349".
void PrintU2( const char *str, uint16_t val )
{
    char valStr[] = "00000";

    DebugPrint2( str );
    usitoa5( val, valStr );
    sendString2( valStr, sizeof(valStr)-1);   //Don't send null termination
}


//Function: PrintUs
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          UINT8 val - value to be printed
//Outputs: none
//
//Description:
//Prints a short string and number out the debug UART. String must be
//null-terminated. Example: PrintUs( "Value=", 158 ); results in "Value=349".
void PrintUs2( const char *str, uint16_t val )
{
  char valStr[] = "0000";

  DebugPrint2( str );
  usitoa4( val, valStr );
  sendString2( &valStr[1], sizeof(valStr)-2);  //Don't send null termination
}


//Function: PrintI
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          INT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and number out the debug UART. String must be null-terminated.
//Example: PrintI( "Value=", 349 ); results in  "Value= 00349".
//Example: PrintI( "Value=", -349 ); results in "Value=-00349".
void PrintI2( const char *str, int16_t val )
{
  char valStr[] = " 00000";
  uint16_t uval;

  DebugPrint2( str );

  if (val < 0)
  {
      valStr[0] = '-';
      uval = (uint16_t)(0-val);
  }
  else
  {
            valStr[0] = '0';
      uval = (uint16_t)val;
  }
  usitoa5( uval, &valStr[1] );
  sendString2( valStr, sizeof(valStr)-1);   //Don't send null termination
}

//Function: PrintIs
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          INT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and number out the debug UART. String must be null-terminated.
//Example: PrintI( "Value=", 349 ); results in  "Value= 349".
//Example: PrintI( "Value=", -349 ); results in "Value=-349".
void PrintIs2( const char *str, int16_t val )
{
  char valStr[] = " 0000";
  int16_t uval;

  DebugPrint2( str );

  if (val < 0)
  {
      uval = (uint16_t)(0-val);
  }
  else
  {
      uval = (uint16_t)val;
  }
  usitoa5( uval, &valStr[0] );
  if (val < 0)
  {
      valStr[0] = '-';
  }
  else
  {
      valStr[0] = '0';
  }
  sendString2( valStr, sizeof(valStr)-1);   //Don't send null termination
}

                        // Print decimal to debug UART
void PrintDEC2(const char *str, uint32_t val)
{
    uint32_t temp = val/1000;
    DebugPrint2( str );
    PrintUs2("",temp);
    DebugPrint2(".");

    temp = (val%1000);
    PrintUs2("",temp);
}
                        // Print hexidecimal to debug UART
void PrintHEX2( const char *str, uint16_t val )
{
  char valStr[] = "00";

  DebugPrint2( str );
  usitoa2h( val, valStr );
  sendString2( valStr, 2 );  //Don't send null termination
}
                        // Print long hexadecimal to debug UART
void PrintHEXL2( const char *str, uint16_t val )
{
  char valStr[] = "000";

  DebugPrint2( str );
    if (val<4096)
    {
  usitoa3h( val, valStr );
  sendString2( valStr, 3 );  //Don't send null termination
    }
    else
    {
        DebugPrint2("xxx");
    }
}
                        // com2pare two strings
uint8_t stringcom2pare2(char *str1 ,char *str2,uint8_t length)
{
    uint8_t i = 0;
    for(i=0;i<length;i++)
    {
        if(str1[i]!=str2[i])
        {
            return 0;
        }
    }
    return 1;
}
                        // Handle UART debug, handle input com2mands
void HandleDebug2( void )
{
    static char buf[MAX_PARSE_BYTES2];
    static uint64_t Timeout;
    static uint8_t direct = 0;
    static uint8_t head=0;
    char recivebuf[16];
    uint8_t revLen = reciveString2(recivebuf);

    int i = 0;
    if (direct)
    {
        if(GetUpTime()>Timeout)
        {
            direct = 0;
            DebugPrint2("\r\nwrite timeout\r\n");
            PrintDIR();
            WriteDIR();
        }
        else
        {
            if(revLen)
            {
                Timeout =  GetUpTime()+50;
            }
            for(i = 0;i<revLen;i++)
            {
                DIRECTWRITEFRAM(recivebuf[i]);
                sendString2( &recivebuf[i], 1 );
            }
        }
    }
    else
    {
    for(i = 0;i<revLen;i++)
    {

    buf[head++]=recivebuf[i];

    if (25 == buf[head-1])
    {
        DebugPrint2("end of file\r\n");
    }
    if ((head < MAX_PARSE_BYTES2) && ('\n' != buf[head-1]) && ('\r' != buf[head-1]))
    {
      sendString2( &buf[head-1], 1 );
    }
    else
    { //Buffer is full or got a newline/CR: check for meaningful messages
            DebugPrint2( "\r\n" );
            if(stringcom2pare2(buf,STORE2,STORE_L2))
            {
                DebugPrint2("Storing\r\n");
                PrintUs2("STRINGLEN: ",head-7);
                DebugPrint2("\r\n");
                Create_NewEntry(buf,head-8);
                Timeout =  GetUpTime()+10000;
                direct = 1;

            }
            else if(stringcom2pare2(buf,DIRECT2,DIRECT_L2))     // Handle com2mands
            {
                DebugPrint2("Directory\r\n");
                PrintDIR();
            }
            else if(stringcom2pare2(buf,SIZE2,SIZE_L2))
            {
                DebugPrint2("MemSize\r\n");
                PrintTotalSize();
            }
            else if(stringcom2pare2(buf,DELETE2,DELETE_L2))
            {
                DebugPrint2("DELETING\r\n");
                DeleteEntry(buf[7]-48);
            }
            else if(stringcom2pare2(buf,READ2,READ_L2))
            {
                DebugPrint2("READING\r\n");
                StartRead(buf[5]-48);
            }
            else if(stringcom2pare2(buf,CLEAR2,CLEAR_L2))
            {
                DebugPrint2("Clearing\r\n");
                ClearFRAM();
            }

            else
            {
                switch (buf[0])
                {
                case 'B':
                    setbpm((buf[1]-48)*100+(buf[2]-48)*10+(buf[3]-48));
                    break;
                case 0x57://'W'
                    writedata((buf[1]-48)*1000+(buf[2]-48)*100+(buf[3]-48)*10+(buf[4]-48),(buf[5]-48)*100+(buf[6]-48)*10+(buf[7]-48));
                    break;
                case 0x52://'R'
                    readdata((buf[1]-48)*1000+(buf[2]-48)*100+(buf[3]-48)*10+(buf[4]-48));
                    break;
                default:
                    break;
                }
            }
            head = 0;
        }
    }
    }
}
