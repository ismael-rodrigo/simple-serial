#include "SerialPort.h"

using namespace std;
SerialPort::SerialPort(const char *portName , const UINT32 baud)
{
    this->connected = false;  
    char port[15];
    strcpy (port ,"\\\\.\\");
    strcat (port ,portName);

    this->handler = CreateFileA(static_cast<LPCSTR>(port),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (this->handler == INVALID_HANDLE_VALUE){
        if (GetLastError() == ERROR_FILE_NOT_FOUND){
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
        else{
            printf("ERROR!!!");
        }
    }

    else {
        DCB dcbSerialParameters = {0};
        dcbSerialParameters.BaudRate = baud;
        dcbSerialParameters.ByteSize = 8;
        dcbSerialParameters.StopBits = ONESTOPBIT;
        dcbSerialParameters.Parity = NOPARITY;
        dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

        if (!SetCommState(handler, &dcbSerialParameters)){
            printf("ALERT: could not set Serial port parameters\n");
        }
        else {
            printf("CONNECTED in %s \n" , portName);
            this->connected = true;
            PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
        }
    }
}




SerialPort::~SerialPort()
{
    if (this->connected){
        this->connected = false;
        CloseHandle(this->handler);
    }
}

int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0){
        if (this->status.cbInQue > buf_size){
            toRead = buf_size;
        }
        else toRead = this->status.cbInQue;
    }

    if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

    return 0;
}

bool SerialPort::writeSerialPort(char *buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*) buffer, buf_size, &bytesSend, 0)){
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    else return true;
}

bool SerialPort::isConnected()
{
    return this->connected;
}