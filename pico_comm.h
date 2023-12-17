#include "serial_comm.h"
#include <windows.h>
#include <ios>

// Convenience class to handle communications with a Raspberry Pi
// Pico. Default initialises to the default Pico serial settings.
class PicoComm: public SerialComm
{
public:
    // Constructor takes in the serial port the Pico is connected to
    // and configures the connection.
    PicoComm(
        LPCSTR ComPort /* String with port to use. */
        ): SerialComm {ComPort}
    {
        OpenSerialConnection();
        SetTimeoutPeriods();

        // Set connection config to default Pico settings.
        SetSerialConnectionConfig(
            CBR_115200,
            8,
            ONESTOPBIT,
            NOPARITY
        );
    }
};