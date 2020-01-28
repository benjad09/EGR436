#include "windows.h"
#include "strings.h"
#include <stdio.h>
#include <stdlib.h>

//#include<iostream>
//#include<conio.h>

HANDLE hMasterCOM;

static struct
{
    char buf[32];
    int head;
    int tail;
}rx;

void WriteUART(char* str,DWORD length)
{
    Sleep(10);
    OVERLAPPED o = {0};
    //.o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DWORD written = 0;
    WriteFile(hMasterCOM,(LPCVOID)str,length,written,&o);
    //while (written != length)
    //{
    //     printf("%d\r\n",written);

    //}


}
void ReadUART(char* str,DWORD length)
{
    Sleep(10);
    OVERLAPPED o = {0};
    //.o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    DWORD written = 0;
    ReadFile(hMasterCOM,(LPCVOID)str,length,written,&o);

//char TempChar; //Temporary character used for reading
//char SerialBuffer[256];//Buffer for storing Rxed Data
//DWORD NoBytesRead;
//int i = 0;
////do
// //{
//   ReadFile( hMasterCOM,           //Handle of the Serial port
//             &TempChar,       //Temporary character
//             sizeof(TempChar),//Size of TempChar
//             &NoBytesRead,    //Number of bytes read
//             NULL);
//
//   str[i] = TempChar;// Store Tempchar into buffer
//   i++;
//  //}
////while (NoBytesRead > 0);
}

//int WriteData(HANDLE handle, BYTE* data, DWORD length, DWORD* dwWritten){
//    OVERLAPPED o = {0};
//    WriteFile(handle, (LPCVOID)data, length, dwWritten, &o);
//    Sleep(1);
//    return 1;
//     int success = 0;
////     OVERLAPPED o = {0};
//     o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
//     if (!WriteFile(handle, (LPCVOID)data, length, dwWritten, &o))
//        {
//            if (GetLastError() == ERROR_IO_PENDING)
//                if (WaitForSingleObject(o.hEvent, INFINITE) == WAIT_OBJECT_0)
//                    if (GetOverlappedResult(handle, &o, dwWritten, FALSE))
//                            success = 1;
//     }
//     else
//        success = 1;
//     if (*dwWritten != length)
//        success = 0;
//     CloseHandle(o.hEvent);
//     return success;
//}


//int ReadData(HANDLE handle, BYTE* data, DWORD length, DWORD* dwWritten)
//{
//     int success = 0;
//     OVERLAPPED o = {0};
//     ReadFile(handle, (LPCVOID)data, length, dwWritten, &o);
//     return 1;
//     o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
//     if (!ReadFile(handle, (LPCVOID)data, length, dwWritten, &o))
//        {
//            if (GetLastError() == ERROR_IO_PENDING)
//                if (WaitForSingleObject(o.hEvent, INFINITE) == WAIT_OBJECT_0)
//                    if (GetOverlappedResult(handle, &o, dwWritten, FALSE))
//                            success = 1;
//     }
//     else
//        success = 1;
//     if (*dwWritten != length)
//        success = 0;
//     CloseHandle(o.hEvent);
//     return success;
//}

void createwritestring(char *str,int address,int value)
{

    sprintf(str,"W%0*d%0*d\r\n",4,address,3,value);//cval);
    return 0;
}

void createreadstring(char* str,int address)
{
    sprintf(str,"R%0*d\r\n",4,address);
    return 0;
}
writeSPI(short address,int data)
{
    char junk[100];
    char datas[10];
    createwritestring(datas,address,data);
    printf("%s",datas);
    WriteUART(datas,10);
    return 0;
}
readSPI(int address)
{
    char junk[100];
    char datas[7];
    createreadstring(datas,address);
    WriteUART(datas,7);
    return 0;
}
void writestrtomem(int address,int length,char* str2){
    int i = 0;
    for (i=0;i<length;i++)
    {
       writeSPI(i+address,str2[i]);
       Sleep(1);
       //printf("%c",str2[i]);
       //Sleep(0.001);
    }

}
void readstrfrommem(int address,int length,char* str)
{
    int i = 0;
    for (i=0;i<length;i++)
    {
       readSPI(i+address);
       //Sleep(0.001);
    }
    readBuffer(length,str);
}
void readBuffer(int len,char* str)
{
    int i = 0,j = 0;
    char readdata;
    char junk[1000];
    do
    {
    ReadUART(&readdata,1);
    printf("%c",readdata);
    Sleep(10);
        if (readdata == 'r'){
            str[i] = 0;
            ReadUART(&readdata,1);
            printf("%c",readdata);
            //Sleep(10);
            str[i] += (readdata-48)*100;
            ReadUART(&readdata,1);
            printf("%c",readdata);
            //Sleep(10);
            str[i] += (readdata-48)*10;
            ReadUART(&readdata,1);
            printf("%c",readdata);
            //Sleep(10);
            str[i] += (readdata-48);
            i++;

        }
    }while(i<len);
}

void main(void){
    //printf("hello world!\r\n");
   // return 0;

     hMasterCOM = CreateFile("\\\\.\\COM3",
                                        GENERIC_READ | GENERIC_WRITE,
                                              0,
                                              0,
                                              OPEN_EXISTING,
                                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                                                0);
    DCB dcbMasterInitState;
    GetCommState(hMasterCOM, &dcbMasterInitState);
    DCB dcbMaster = dcbMasterInitState;
    dcbMaster.BaudRate = 115200;
    dcbMaster.Parity = NOPARITY;
    dcbMaster.ByteSize = 8;
    dcbMaster.StopBits = ONESTOPBIT;
    SetCommState(hMasterCOM, &dcbMaster);

    char junk[100];
    char newline[] = " \r\n";
    char datain[32];
    char str[100];
    char printedsting[] ="Test2";
    int i = 0;
    WriteUART("Hello",5);

    char readdata;
    char input;

    WriteUART(newline,3);

    writestrtomem(0,6,printedsting);

    Sleep(1000);


    readstrfrommem(0,6,datain);

    printf("\r\n%s",datain);

    while(1)
    {
        //ReadData(hMasterCOM,&readdata,1,junk);
        //printf("%c",readdata);
        //Sleep(1000);
        //printf("1sec\n");
    }

    return 0;
}
