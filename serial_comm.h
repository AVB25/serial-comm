#include <iostream>
#include <windows.h>
#include <ios>

#ifndef SERIAL_COMM
#define SERIAL COMM

// Class that handles serial communications to and from a device connected
// to a serial port (typically COM port).
class SerialComm
{
private:
    HANDLE hSerial;                      /* Handle to the serial connection */
    LPCSTR ComPort;                      /* Serial port to connect to */
    DCB dcbSerialParams = {0};           /* Stores the state of the serial port */
    COMMTIMEOUTS timeouts = {0};         /* Struct storing timeout settings */
    BOOL bConnectionInitialised = FALSE; /* Flag storing whether handle has been initialised */
    

public:
    // Constructor just stores value of COM port to use.
    SerialComm(
        LPCSTR ComPort /* String with ComPort to use. */
    );

    // Set port to connect to
    void SetComPort(
        LPCSTR ComPort /* String with ComPort to use. */
    );

    /* GETTERS */

    // Get serial port connected to
    LPCSTR GetComPort();

    // Get the connection initialised flag
    BOOL GetConnectionInitialised();

    // Get handle to serial connection.
    HANDLE GetHandle();

    // Get DCB settings
    DCB GetDCBSerialParams();

    // Get timeout params
    COMMTIMEOUTS GetTimeouts();

    /*  OPEN AND CLOSE CONNECTIONS  */

    bool OpenSerialConnection();

    bool CloseSerialConnection();

    /*  CONFIGURE SERIAL CONNECTION  */

    // Set timeout periods in ms.
    bool SetTimeoutPeriods(
        int ReadIntervalTimeout = 50,        /* Maximum time between read chars. */
        int ReadTotalTimeoutConstant = 50,   /* Constant in ms. */
        int ReadTotalTimeoutMultiplier = 10, /* Multiplier of characters. */
        int WriteTotalTimeoutConstant = 50,  /* Constant in ms. */
        int WriteTotalTimeoutMultiplier = 10 /* Multiplier of characters. */
    );

    // Initialise DCB struct to store config of serial connection
    bool SetSerialConnectionConfig(int BaudRate, int ByteSize, int StopBits, int Parity);

    /*  READ AND WRITE FROM PORT  */

    bool ReadBytes(
        LPVOID bpReadBuffer,  /* Pointer to buffer to store the bytes read from the port. */
        DWORD dwLengthBuffer, /* Length of the buffer. */
        LPDWORD dwpBytesRead  /* Pointer to int that stores the number of bytes read. */
    );

    bool WriteBytes(
        LPCVOID bpData,         /* Pointer to the array of bytes to write */
        DWORD dwSizeData,       /* Size of data to write */
        LPDWORD dwpBytesWritten /* Pointer to buffer storing num bytes written */
    );
};

// const DWORD dwReadBuffLength = 10;
// const DWORD dwWriteBuffLength = 3;

// bool ReadPort(BYTE (*bBuffer)[dwReadBuffLength + 1], SerialComm* SerialManager);

// bool WriteToPort(BYTE (*bData)[dwWriteBuffLength + 1], SerialComm* SerialManager);

#endif