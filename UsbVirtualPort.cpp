#include <Windows.h>
#include <stdio.h>

int main() {
    HANDLE hComm;                           // Handle to the Serial port
    char  ComPortName[] = "\\\\.\\COM3";    // Name of the Serial port
    DWORD  dwEventMask;                     // Event mask to trigger
    char   TempChar;                        // Temperory Character
    char   SerialBuffer[256];               // Buffer Containing Rxed Data
    DWORD  NoBytesRead;                     // Bytes read by ReadFile()
    int i = 0;

/*------------------------- Opening the Serial Port --------------------------*/
     hComm = CreateFile(
        ComPortName,                        // Name of the Port to be Opened
        GENERIC_READ | GENERIC_WRITE,       // Read/Write Access
        0,                                  // No Sharing, ports cant be shared
        NULL,                               // No Security
        OPEN_EXISTING,                      // Open existing port only
        0,                                  // Non Overlapped I/O
        NULL                                // Null for Comm Devices
     );

    if (hComm == INVALID_HANDLE_VALUE) {
        printf("\n    Error! - Port %s can't be opened\n", ComPortName);
    }
    else {
        printf("\n    Port %s Opened\n ", ComPortName);
    }

/*----------------- Setting the Parameters for the SerialPort ----------------*/
    DCB dcbSerialParams       = { 0 };      // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    //retreives  the current settings
    if (GetCommState(hComm, &dcbSerialParams) == FALSE) { 
        printf("\n    Error! in GetCommState()");
    }

    dcbSerialParams.BaudRate = CBR_115200;  // Setting BaudRate = 115200
    dcbSerialParams.ByteSize = 8;           // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;  // Setting StopBits = 1
    dcbSerialParams.Parity   = NOPARITY;    // Setting Parity   = None

    //Configuring the port according to settings in DCB 
    if (SetCommState(hComm, &dcbSerialParams) == FALSE) {
        printf("\n    Error! in Setting DCB Structure");
    }
    else { 
        // Successfull display the contents of the DCB Structure
        printf("\n\n    Setting DCB Structure Successfull\n");
        printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d", dcbSerialParams.Parity);
    }

/*----------------------------- Setting Timeouts -----------------------------*/
    COMMTIMEOUTS timeouts                = { 0 };
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
        printf("\n\n    Error! in Setting Time Outs");
    }
    else {
        printf("\n\n    Setting Serial Port Timeouts Successfull");
    }

/*--------------------------- Setting Receive Mask ---------------------------*/
    //Configure Windows to Monitor the serial device for Character Reception
    if (SetCommMask(hComm, EV_RXCHAR) == FALSE) {
        printf("\n\n    Error! in Setting CommMask");
    }
    else {
        printf("\n\n    Setting CommMask successfull");
    }

/*------------------------- Setting WaitComm() Event -------------------------*/
/*----------- Program will Wait here till a Character is received ------------*/
    printf("\n\n    Waiting for Data Reception");

    if (WaitCommEvent(hComm, &dwEventMask, NULL) == FALSE) {
        //Wait for the character to be received
        printf("\n    Error! in Setting WaitCommEvent()");
    }
    else {
        //Read the RXed data using ReadFile();
        printf("\n\n    Characters Received");
        do {
            ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
            SerialBuffer[i] = TempChar;
            i++;
        } while (NoBytesRead > 0);

        /*------------------- Printing the RXed String to Console --------------------*/
        printf("\n\n    ");
        int j = 0;
        for (j = 0; j < i - 1; j++) {
            // j < i-1 to remove the dupliated last character
            printf("%c", SerialBuffer[j]);
        }
    }

/*--------------------------------- Finish -----------------------------------*/
    CloseHandle(hComm); //Closing the Serial Port
    printf("\n==========================================\n");

    getchar();
    return 0;
}
