/*
 * FRAM.c
 *
 *  Created on: Jan 27, 2020
 *      Author: bendr
 */

#include "mySPI.h"
#include "UART.h"
#include "APP.h"
#include "Timer.h"
#include "msp.h"
#include "FRAM.h"

static struct
{
    uint8_t entry;
    uint8_t selected;
    uint8_t lastentry;
    uint16_t totalsize;
    uint16_t address;
    char data[BLOCKSIZE];
    framstate_t  state;
}mem;


static struct
{
    uint16_t size;
    uint16_t address;
}del;

static struct
{
    char name[NAME_LEN];
    uint16_t address;
    uint16_t length;

}entries[ENTRIES_N];

static void FRAMFSM(void);
static void ReadDIR(void);


void InitFRAM(void)
{
    mem.state = FR_IDLE;
    ReadDIR();
}
void HandleFRAM(void)
{
    FRAMFSM();
}
void writename(char* NAME,uint8_t entry,uint8_t len)
{
    int16_t j = 0;
    for(j=0;j<NAME_LEN;j++)
    {
        if(j<len)
        {
            entries[entry].name[j]=NAME[j];
        }
        else
        {
            entries[entry].name[j] = '\0';
        }
    }
}

void ClearFRAM()
{
    int16_t i;
    //mem.state = FR_CLEAR;
    mem.address = 0;
    mem.entry = 0;
    char defNAME[] = DEFAULT_NAME;
    for(i=0;i<ENTRIES_N;i++)
    {
        entries[i].address = 0;
        entries[i].length = 0;
        writename(defNAME,i,DEFAULT_NAME_L);

    }
    WriteDIR();

}
void WriteDIR(void)
{
    mem.entry = 0;
    mem.address = 0;
    mem.state = FR_WRITEDIR;
    //ReadDIR();
}
void ReadDIR(void)
{
    mem.entry = 0;
    mem.address = 0;
    mem.state = FR_READDIR;
}
void PrintDIR(void)
{

    uint8_t i;
    for(i = 0;i<ENTRIES_N;i++)
    {
    DebugPrint(entries[i].name);
    PrintHEXL(" ",entries[i].address);
    PrintHEXL(" ",entries[i].length);
    DebugPrint("\r\n");
    }
    ReadDIR();
}
void Create_NewEntry(char* str,uint8_t len)
{
    uint16_t i;
    mem.selected = mem.lastentry;
    writename(str+6,mem.selected,len+1);
    entries[mem.selected].address = 320;
    for(i=0;i<ENTRIES_N;i++)
    {
        entries[mem.selected].address += entries[i].length;
    }
    entries[mem.selected].length = 0;
    WriteDIR();
    //mem.address = entries[mem.selected].address;
}
void DIRECTWRITEFRAM(char data)
{
    writedata(mem.address++,data);
    entries[mem.selected].length += 1;
    //PrintHEXL("A: ",mem.address);
    //DebugPrint("\r\n");
    //PrintHEX(" E: ",mem.selected);
    //PrintHEX(" L: ",entries[mem.selected].length);
    //DebugPrint("!\r\n");
}

void StartRead(uint8_t instance)
{
    mem.selected = instance;
    mem.address = entries[mem.selected].address;
    PrintU("printing ",mem.selected);
    DebugPrint("\r\n");
    readdata(mem.address++);
    mem.state = FR_READ;
}
void DeleteEntry(uint8_t instance)
{
    uint8_t i = 0,j=0;
    del.address = entries[instance].address;
    del.size = entries[instance].length;
    for(i = 0;i<ENTRIES_N;i++)
    {
        if(instance<i){
            for(j=0;j<NAME_LEN;j++)
            {
                entries[i-1].name[j]=entries[i].name[j];
            }
            if(entries[i].address > 0)
            {
                entries[i-1].address = entries[i].address-del.size;
            }
            else
            {
                entries[i-1].address = 0;
            }

            entries[i-1].length = entries[i].length;

        }
    }
    mem.address = del.address+del.size;
    readdata(mem.address++);
    mem.state = FR_DELETE;
}




void PrintTotalSize(void)
{
    PrintHEXL("Total size: ",mem.totalsize);
    DebugPrint("\r\n");
}


void FRAMFSM()
{
    char incoming[BLOCKSIZE];
    uint16_t i = 0;
    switch(mem.state){
    case FR_IDLE:
        break;
    case FR_CLEAR:
        if(!SPIbusy())
        {
//            blockdelay = GetUpTime()+WRITEDELAY;
//            if(mem.address < MAX_ADDRESS)
//            {
//            for (i = 0;i<BLOCKSIZE;i++)
//            {
//            writedata(mem.address++,0);
//            }
//            }
//            else
//            {
                //WriteDIR();
//            }

        }
        break;
    case FR_READ:
        if(!SPIbusy())
          {
            reciveSPI(incoming);
            sendString(&incoming[0],1);
            //PrintHEXL("A: ",mem.address);
            //DebugPrint("\r\n");


           if(mem.address < (entries[mem.selected].address+entries[mem.selected].length))
           {

               readdata(mem.address++);
           }
           else
           {
               mem.state = FR_IDLE;
               DebugPrint("\r\n");
               DebugPrint("done!\r\n");
           }
          }
        break;
    case FR_DELETE:
        if(!SPIbusy())
          {
            reciveSPI(incoming);
            //sendString(&incoming[0],1);
            //PrintHEXL("A: ",mem.address);
            //DebugPrint("\r\n");
            writedata(mem.address-del.size-1,incoming[0]);


           if(mem.address < MAX_ADDRESS)
           {

               readdata(mem.address++);
           }
           else
           {
               WriteDIR();
               //mem.state = FR_IDLE;
               DebugPrint("done!\r\n");
           }
          }
        break;
    case FR_WRITEDIR:
        if(!SPIbusy())
        {
            for (i = 0;i<BLOCKSIZE;i++)
            {
                if(i<NAME_LEN)
                {
                    writedata(mem.address++,entries[mem.entry].name[i]);
                }
                else if (i == 28)
                {
                    writedata(mem.address++,(entries[mem.entry].address>>8)&0xff);
                }
                else if (i == 29)
                {
                    writedata(mem.address++,entries[mem.entry].address&0xff);
                }
                else if (i == 30)
                {
                    writedata(mem.address++,(entries[mem.entry].length>>8)&0xff);
                }
                else if (i == 31)
                {
                    writedata(mem.address++,entries[mem.entry].length&0xff);
                }
            }
            mem.entry++;
            if(mem.entry == ENTRIES_N)
            {
                //mem.state = FR_IDLE;
                mem.address = entries[mem.selected].address;
                DebugPrint("write done!\r\n");
                ReadDIR();
            }

        }

         break;
    case FR_READDIR:
        if(!SPIbusy()){
            if (mem.entry == 0)
            {
                reciveSPI(incoming);
                for (i = 0;i<BLOCKSIZE;i++)
                {
                    readdata(mem.address++);
                }
                mem.entry++;
            }
            else
            {
                if(peakSPI() >= BLOCKSIZE)
                {
                    reciveSPI(incoming);

                    for (i = 0;i<NAME_LEN;i++)
                    {
                       entries[mem.entry-1].name[i] = incoming[i];
                       //PrintHEX("SPI->",incoming[i]);
                       //DebugPrint("\r\n");
                    }

                    entries[mem.entry-1].address = (incoming[28])<<8;//(uint16_t)((uint16_t)incoming[28]<<8+(uint16_t)incoming[29]);
                    entries[mem.entry-1].address += incoming[29];
                    entries[mem.entry-1].length = (incoming[30])<<8;
                    entries[mem.entry-1].length += incoming[31];
                    //PrintHEXL("",entries[mem.entry-1].address );
                    if(mem.entry < ENTRIES_N)
                    {
                        for (i = 0;i<BLOCKSIZE;i++)
                        {
                            readdata(mem.address++);
                        }
                    }
                    mem.entry++;
                    if(mem.entry > ENTRIES_N)
                    {
                        for(i = 0;i<BLOCKSIZE;i++)
                        {
                            if(entries[i].length == 0)
                            {
                                mem.lastentry = i;
                                break;
                            }
                        }
                        mem.totalsize = 0;
                        for(i = 0;i<ENTRIES_N;i++)
                        {
                            mem.totalsize += entries[i].length;
                        }
                        mem.address = entries[mem.selected].address;
                        PrintHEX("lastentry",mem.lastentry);
                        mem.state = FR_IDLE;
                        DebugPrint("done!\r\n");
                    }
                }
            }
        }

        break;
    default:
        break;
    }
}
