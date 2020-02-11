/*
 * FRAM.h
 *
 *  Created on: Jan 27, 2020
 *      Author: bendr
 */

#ifndef FRAM_H_
#define FRAM_H_

#define NAME_LEN              28
#define BLOCKSIZE             32
#define WRITEDELAY            50
#define ENTRIES_N             10
#define DIROFF


#define MAX_ADDRESS           4095

//#define DEFAULT_NAME         "abcdefghijklmnopqrstuvwxyz1"
//#define DEFAULT_NAME_L        28

#define DEFAULT_NAME         "NADA"
#define DEFAULT_NAME_L        5

typedef enum
{
    FR_IDLE,
    FR_CLEAR,
    FR_READ,
    FR_DELETE,
    FR_READDIR,
    FR_WRITEDIR,
}framstate_t;

void InitFRAM(void);
void HandleFRAM(void);
void ClearFRAM(void);
void PrintDIR(void);
void WriteDIR(void);
void Create_NewEntry(char* str,uint8_t len);
void DIRECTWRITEFRAM(char data);
void StartRead(uint8_t instance);
void PrintTotalSize(void);
void DeleteEntry(uint8_t instance);



#endif /* FRAM_H_ */
