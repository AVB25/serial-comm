#include <iostream>
#include <windows.h>
#include <ios>
#include "serial_comm.h"

// Constructor just stores value of COM port to use.
SerialComm::SerialComm(
    LPCSTR ComPort /* String with ComPort to use. */
    ) : ComPort(ComPort)
{
}

// Set port to connect to
void SerialComm::SetComPort(
    LPCSTR ComPort /* String with ComPort to use. */
)
{
    ComPort = ComPort;
}

/* GETTERS */

// Get serial port connected to
LPCSTR SerialComm::GetComPort()
{
    return ComPort;
}

// // Get the connection initialised flag
BOOL SerialComm::GetConnectionInitialised()
{
    return bConnectionInitialised;
}

// Get handle to serial connection.
HANDLE SerialComm::GetHandle()
{
    if (!GetConnectionInitialised())
    {
        // Connection handle holds is not initialised
        printf("Handle points to uninitialised connection");
    }
    return hSerial;
}

// Get DCB settings
DCB SerialComm::GetDCBSerialParams()
{
    return dcbSerialParams;
}

// Get timeout params
COMMTIMEOUTS SerialComm::GetTimeouts()
{
    return timeouts;
}

/*  OPEN AND CLOSE CONNECTIONS  */

bool SerialComm::OpenSerialConnection()
{
    hSerial = CreateFile(
        ComPort,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            // Serial port doesn't exist. Inform user.
            std::cout << "Serial port " << ComPort << " does not exist.\n";
            return 1;
        }
        // Some other error occurred.
        printf("Some error occurred when initialising port.\n");
        return FALSE;
    }

    // Raise connection initialised flag
    bConnectionInitialised = TRUE;
    return TRUE;
}

bool SerialComm::CloseSerialConnection()
{
    if (!CloseHandle(hSerial))
    {
        // Error in closing the serial connection
        std::cerr << "Error closing the serial connection.\n";
        return FALSE;
    };

    // Lower connection initialised flag
    bConnectionInitialised = FALSE;
    return TRUE;
}

/*  CONFIGURE SERIAL CONNECTION  */

// Set timeout periods in ms.
bool SerialComm::SetTimeoutPeriods(
    int ReadIntervalTimeout,            /* Maximum time between read chars. */
    int ReadTotalTimeoutConstant,       /* Constant in ms. */
    int ReadTotalTimeoutMultiplier,     /* Multiplier of characters. */
    int WriteTotalTimeoutConstant,      /* Constant in ms. */
    int WriteTotalTimeoutMultiplier     /* Multiplier of characters. */
)
{
    timeouts.ReadIntervalTimeout = ReadIntervalTimeout;
    timeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
    timeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
    timeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
    timeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

    if (!bConnectionInitialised)
    {
        // Connection has not been initialised
        printf("Serial connection not initialised.\n");
        return FALSE;
    }

    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        // Error occurred setting timeouts
        printf("Error setting timeouts.\n");
        return FALSE;
    }
    return TRUE;
}

// Initialise DCB struct to store config of serial connection
bool SerialComm::SetSerialConnectionConfig(int BaudRate, int ByteSize, int StopBits, int Parity)
{
    if (!bConnectionInitialised)
    {
        // Connection has not been initialised
        printf("Serial connection not initialised.\n");
        return FALSE;
    }

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        // Error getting state
        printf("Error getting state from serial handle.\n");
        return FALSE;
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    // Set params to RP Pico default
    dcbSerialParams.BaudRate = BaudRate;
    dcbSerialParams.ByteSize = ByteSize;
    dcbSerialParams.StopBits = StopBits;
    dcbSerialParams.Parity = Parity;

    // Set config of serial connection
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        // Error setting serial port state
        printf("Error setting serial port state\n");
        return FALSE;
    }
    return TRUE;
}

/*  READ AND WRITE FROM PORT  */

bool SerialComm::ReadBytes(
    LPVOID bpReadBuffer,  /* Pointer to buffer to store the bytes read from the port. */
    DWORD dwLengthBuffer, /* Length of the buffer. */
    LPDWORD dwpBytesRead  /* Pointer to int that stores the number of bytes read. */
)
{
    if (!ReadFile(hSerial, bpReadBuffer, dwLengthBuffer, dwpBytesRead, NULL))
    {
        // Error occurred while reading the buffer.
        std::cerr << "Error occurred while reading buffer.\n";
        return FALSE;
    }
    return TRUE;
}

bool SerialComm::WriteBytes(
    LPCVOID bpData,         /* Pointer to the array of bytes to write */
    DWORD dwSizeData,       /* Size of data to write */
    LPDWORD dwpBytesWritten /* Pointer to buffer storing num bytes written */
)
{
    if (!WriteFile(hSerial, bpData, dwSizeData, dwpBytesWritten, NULL))
    {
        // Error ocurred while writing the buffer.
        std::cerr << "Error occurred while writing to the buffer.\n";
        return FALSE;
    }
    return TRUE;
}

