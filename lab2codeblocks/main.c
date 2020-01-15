#include "windows.h"
#include "strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

char in_string[25] = {0};
int in_string_pos = 0;
int string_len = 0;
int index = 0;

int WriteData(HANDLE handle, BYTE* data, DWORD length, DWORD* dwWritten)
{
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

void main(void)
{
    HANDLE hMasterCOM = CreateFile("\\\\.\\COM6",
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

    WriteData(hMasterCOM,datas,6,junk);

    printf("Enter Commands: \n");
    while(1)
    {
        input = getch();        // Get the input character
        printf("%c", input);    // Print input back to user
        string_len = strlen(in_string);     // Get string length


        if (input == 13)        // User has hit enter (carriage return encountered)
        {
            printf("\n");
            //printf("%s\n", in_string);

            in_string[in_string_pos+1] = '\r';  //String formatting
            in_string[in_string_pos+2] = '\n';

                                                // Send string through COM port
            WriteData(hMasterCOM,in_string,string_len+2,junk);

            in_string_pos = 0;                  // Reset string position
            for(index = 0; index < 24; index++) // Clear the string
                {
                in_string[index] = 0;
                }
        }
        //else if ((input >= 65)&(input <= 90))
        else
        {
            in_string[in_string_pos] = input;   // Add input character to string
            in_string_pos++;                    // Increment string position index
        }
    }

    return 0;
}
