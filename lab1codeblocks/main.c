#include "windows.h"
#include "strings.h"
#include <stdio.h>
#include <stdlib.h>
//#include<iostream>
#include<conio.h>

int WriteData(HANDLE handle, BYTE* data, DWORD length, DWORD* dwWritten){
     int success = 0;
     OVERLAPPED o = {0};
     o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
     if (!WriteFile(handle, (LPCVOID)data, length, dwWritten, &o))
        {
            if (GetLastError() == ERROR_IO_PENDING)
                if (WaitForSingleObject(o.hEvent, INFINITE) == WAIT_OBJECT_0)
                    if (GetOverlappedResult(handle, &o, dwWritten, FALSE))
                            success = 1;
     }
     else
        success = 1;
     if (*dwWritten != length)
        success = 0;
     CloseHandle(o.hEvent);
     return success;
}

void makeprintstring(char *str,int value)
{
    if(value>999){
        sprintf(str,"B999\r\n");
    }
    else if(value<10)
    {
        sprintf(str,"B00%d\r\n",value);
    }
    else if(value<100)
    {
       sprintf(str,"B0%d\r\n",value);
    }
    else
    {
        sprintf(str,"B%d\r\n",value);
    }
}


void main(void){
    //printf("hello world!\r\n");
   // return 0;

    HANDLE hMasterCOM = CreateFile("\\\\.\\COM3",
                                        GENERIC_READ | GENERIC_WRITE,
                                              0,
                                              0,
                                              OPEN_EXISTING,
                                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                                                0);
    DCB dcbMasterInitState;
    GetCommState(hMasterCOM, &dcbMasterInitState);
    DCB dcbMaster = dcbMasterInitState;
    dcbMaster.BaudRate = 9600;
    dcbMaster.Parity = NOPARITY;
    dcbMaster.ByteSize = 8;
    dcbMaster.StopBits = ONESTOPBIT;
    SetCommState(hMasterCOM, &dcbMaster);

    int bpm = 60;

    char datas[] = "B060\n\r";
    char junk[100];
    char input;
    //int i = 0;
    //long j = 0;
    //for (i = 0;i<8;i++)
    //{
    //   printf("%d\r\n",WriteData(hMasterCOM,&datas[i],1,junk));
    //   for(j=0;j<100000;j++);
    //}
    //printf("%d\r\n",WriteData(hMasterCOM,datas,6,junk));

    makeprintstring(datas,bpm);
    WriteData(hMasterCOM,datas,6,junk);
    //printf("%s",datas);
    while(1)
    {
        input = getch();
        printf("%c\r\n",input);
        if(input == 'u')
        {
            bpm += 2;
            makeprintstring(datas,bpm);
            WriteData(hMasterCOM,datas,6,junk);
        }
        if(input == 'd')
        {
            bpm -= 2;
            makeprintstring(datas,bpm);
            WriteData(hMasterCOM,datas,6,junk);
        }
        if(input == 'r')
        {
            bpm = 60;
            makeprintstring(datas,bpm);
            WriteData(hMasterCOM,datas,6,junk);
        }
    }

    return 0;
}
