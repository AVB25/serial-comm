#include "pico_comm.h"
# include <string>

int main(){
    std::string CommPort;
    std::cout << "Insert serial port to use: ";
    std::cin >> CommPort;
    PicoComm SerialManager = PicoComm(CommPort.c_str());
    // PicoComm SerialManager("COM6");

    // Buffers to store byte strings when reading and writing to port.
    BYTE bReadBuffer[dwReadBuffLength + 1] = {0};
    BYTE bWriteBuffer[dwWriteBuffLength + 1];

    do{
        // Choose whether to read or write to the buffer by inputting '1' or '2' respectively.
        std::cout << "To read from port, input '1'. To write to port, input '2'. To exit, input '3': ";
        std::string _Input;
        std::cin >> _Input;
        int iReadOrWrite = (int) _Input[0] - '0';   /* Convert user input to int */

        if ((iReadOrWrite < 0) | (iReadOrWrite > 9)){
            // First char of input can't be converted to an int, it's not a number.
            std::cerr << "Input is not a number.\n";
            continue;
        }

        if(iReadOrWrite == 1){  /* Read from port */
            ReadPort(&bReadBuffer, &SerialManager);
        }
        else if(iReadOrWrite == 2){ /* Write to port */
            std::cout << "Enter " << dwWriteBuffLength << " characters to write: ";
            std::cin >> _Input;
            for (int i=0; i < dwWriteBuffLength; i++){
                // (Somewhat) safely write input to buffer
                if (_Input[i] == '\0')
                {
                    printf("Only entered %i characters.\n", i);
                    continue;
                }
                bWriteBuffer[i] = (BYTE)_Input[i];
            }
            bWriteBuffer[dwWriteBuffLength] = NULL;

            WriteToPort(&bWriteBuffer, &SerialManager);
        }
        else if(iReadOrWrite == 3){ /* Exit */
            SerialManager.CloseSerialConnection();
            break;
        }
        else{   /* Invalid input */
            std::cout << "Invalid input.\n";
        }
    } while(true);
}

