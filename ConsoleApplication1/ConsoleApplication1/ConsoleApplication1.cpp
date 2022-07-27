#include <iostream>
#include <windows.h>
#include <string>

#define TextBufferSize  256

using namespace std;

HANDLE connectedPort;
int selectedPort = 7;
bool isConnected = false;
int targetBaudRate = 57600;

int serialBegin(int BaudRate, int Comport);
void connectRequest(void);
void SerialWrite(void);
char* Buffer = new char[256];

int main()
{
    connectRequest();
    SerialWrite();
    connectRequest();
    return 1;
}


int serialBegin(int BaudRate, int Comport) 
{
    //CloseHandle(connectedPort);

    connectedPort = CreateFileA(
        ("\\\\.\\COM" + to_string(Comport)).c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (connectedPort == INVALID_HANDLE_VALUE)  return -4;  //No port
    DCB SerialParams;
    SerialParams.DCBlength = sizeof(SerialParams);
    if (!GetCommState(connectedPort, &SerialParams)) 
    return -3; //GetState error

    SerialParams.BaudRate = BaudRate;
    SerialParams.ByteSize = 8;
    SerialParams.StopBits = ONESTOPBIT;
    SerialParams.Parity = NOPARITY;
    if (!SetCommState(connectedPort, &SerialParams)) return -2; //SetState error

    COMMTIMEOUTS SerialTimeouts;
    SerialTimeouts.ReadIntervalTimeout = 1;
    SerialTimeouts.ReadTotalTimeoutConstant = 1;
    SerialTimeouts.ReadTotalTimeoutMultiplier = 1;
    SerialTimeouts.WriteTotalTimeoutConstant = 1;
    SerialTimeouts.WriteTotalTimeoutMultiplier = 1;
    if (!SetCommTimeouts(connectedPort, &SerialTimeouts)) return -1; //SetTimeouts error

    return 0;
}

void connectRequest(void) {

    if (isConnected) {
        CloseHandle(connectedPort);
        isConnected = false;
        cout << "disconnected\n";
        return;
    }

    switch (serialBegin(targetBaudRate, selectedPort))
    {
    case -4: cout << "error\n";
    case -3: cout << "error\n";
    case -2: cout << "error\n";
    case -1: cout << "error\n";
    case 0: 
        cout << "connected\n";
        isConnected = true;
        return;
    }

    CloseHandle(connectedPort);
}

void strСpy(char* str1, char* str2)
{//               -------->
    int quant = strlen(str1);
    for (int i = 0; i <= quant; i++)
        str2[i] = str1[i];
}

void addEnld(char* str)
{
    int quant = strlen(str);
    str[quant] = '\n';
    str[quant + 1] = 0;
}

void SerialWrite() {
    if (!isConnected) return;
    DWORD BytesIterated;

    do
    {
        char* Buffer = new char[256];
        cin.getline(Buffer, 256); 
        addEnld(Buffer);
        WriteFile(connectedPort, Buffer, strlen(Buffer), &BytesIterated, NULL);
        delete[] Buffer;

        cout << "\n--------------\n";
    } while (1);
}