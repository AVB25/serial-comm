#include <iostream>
#include <windows.h>
#include <ios>

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
        ) : ComPort(ComPort)
    {
    }

    // Set port to connect to
    void SetComPort(
        LPCSTR ComPort /* String with ComPort to use. */
    )
    {
        SerialComm::ComPort = ComPort;
    }

    /* GETTERS */

    // Get serial port connected to
    LPCSTR GetComPort()
    {
        return ComPort;
    }

    // Get the connection initialised flag
    BOOL GetConnectionInitialised()
    {
        return bConnectionInitialised;
    }

    // Get handle to serial connection.
    HANDLE GetHandle()
    {
        if (!GetConnectionInitialised())
        {
            // Connection handle holds is not initialised
            printf("Handle points to uninitialised connection");
        }
        return hSerial;
    }

    // Get DCB settings
    DCB GetDCBSerialParams()
    {
        return dcbSerialParams;
    }

    // Get timeout params
    COMMTIMEOUTS GetTimeouts()
    {
        return timeouts;
    }

    /*  OPEN AND CLOSE CONNECTIONS  */

    bool OpenSerialConnection()
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

    bool CloseSerialConnection()
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
    bool SetTimeoutPeriods(
        int ReadIntervalTimeout = 50,        /* Maximum time between read chars. */
        int ReadTotalTimeoutConstant = 50,   /* Constant in ms. */
        int ReadTotalTimeoutMultiplier = 10, /* Multiplier of characters. */
        int WriteTotalTimeoutConstant = 50,  /* Constant in ms. */
        int WriteTotalTimeoutMultiplier = 10 /* Multiplier of characters. */
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
    bool SetSerialConnectionConfig(int BaudRate, int ByteSize, int StopBits, int Parity)
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

    bool ReadBytes(
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

    bool WriteBytes(
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
};

const DWORD dwReadBuffLength = 10;
const DWORD dwWriteBuffLength = 3;

bool ReadPort(BYTE (*bBuffer)[dwReadBuffLength + 1], SerialComm* SerialManager){
    // Read port managed by the SerialManager and store the output to
    // the passed buffer.
    int iIn;
    std::cout << "Press any key to read buffer. Input '-1' to exit: ";
        std::cin >> iIn;
        if (iIn != -1){
            DWORD dwBytesRead;
            (*SerialManager).ReadBytes(bBuffer, dwReadBuffLength, &dwBytesRead);
            if (dwBytesRead != dwReadBuffLength){
                // Haven't read the desired number of bytes. For now, error.
                std::cerr << "Requested " << dwReadBuffLength << " bytes, but got " << dwBytesRead << ".\n";
                return FALSE;
            }

            printf("Bytes read: ");
            for (int i = 0; i < sizeof(*bBuffer); i++){
                printf("%c ", (*bBuffer)[i]);
                printf("%u ", (*bBuffer)[i]);
            }
            printf("\n");
            return TRUE;
        }
        return TRUE;
}

bool WriteToPort(BYTE (*bData)[dwWriteBuffLength + 1], SerialComm* SerialManager){
    // Write the contents of the passed byte array to the port managed by
    // the SerialManager.

    DWORD dwBytesWritten;
    (*SerialManager).WriteBytes(bData, dwWriteBuffLength, &dwBytesWritten);
    if (dwBytesWritten != dwReadBuffLength){
        // Havent' written the desired number of bytes.
        std::cerr << "Gave " << dwWriteBuffLength << " bytes to write, but only wrote ";
        std::cerr << dwBytesWritten << ".\n";
        return FALSE;
    }
    return TRUE;
}
