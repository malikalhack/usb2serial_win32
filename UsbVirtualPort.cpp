#include <Windows.h>
#include <stdio.h>
#include <cstdlib>

HANDLE hCommR, hCommW;   // Handle to the Serial ports

int finish_program(int status) {
    CloseHandle(hCommR); //Closing the serial port for reading
    CloseHandle(hCommW); //Closing the serial port for writing
    printf("\n======================== Finish =========================\n");

    getchar();
    return status;
}

int main() {
    char   rComPortName[] = "\\\\.\\COM3";  // Serial port name to read
    char   wComPortName[] = "\\\\.\\COM4";  // Serial port name to write
    DWORD  dwEventMask;                     // Event mask to trigger
    char   SerialBuffer[256];               // Buffer Containing Rxed Data
    DWORD  NumBytesRead;                     // Bytes read by ReadFile()

    char   lpBuffer[] = "Hello world!";     // lpBuffer should be char or byte array
    DWORD  dNumOfBytes2Write;               // Number of bytes to write to the port
    DWORD  dNumOfBytesWritten = 0;          // Number of bytes written to the port

    dNumOfBytes2Write = sizeof(lpBuffer);   // Calculating the number of bytes to write to a port
    int i = 0;

/*------------------------- Opening the Serial Port --------------------------*/
    printf("\n========================= Start =========================\n\n");
    hCommR = CreateFile(
        rComPortName,                       // Name of the Port to be Opened
        GENERIC_READ | GENERIC_WRITE,       // Read/Write Access
        0,                                  // No Sharing, ports cant be shared
        NULL,                               // No Security
        OPEN_EXISTING,                      // Open existing port only
        0,                                  // Non Overlapped I/O
        NULL                                // Null for Comm Devices
     );

    if (hCommR == INVALID_HANDLE_VALUE) {
        printf("Error >>> Port %s can't be opened\n", rComPortName);
        return finish_program(EXIT_FAILURE);
    }
    else printf("Port %s is opened\n", rComPortName);

    hCommW = CreateFile(
        wComPortName,                       // Name of the Port to be Opened
        GENERIC_READ | GENERIC_WRITE,       // Read/Write Access
        0,                                  // No Sharing, ports cant be shared
        NULL,                               // No Security
        OPEN_EXISTING,                      // Open existing port only
        0,                                  // Non Overlapped I/O
        NULL                                // Null for Comm Devices
    );

    if (hCommW == INVALID_HANDLE_VALUE) {
        printf("Error >>> Port %s can't be opened\n", wComPortName);
        return finish_program(EXIT_FAILURE);
    }
    else printf("Port %s is opened\n", wComPortName);

/*----------------- Setting the Parameters for the SerialPort ----------------*/
    DCB dcbSerialParamsR = { 0 }; // Initializing DCB structure for the read port
    DCB dcbSerialParamsW = { 0 }; // Initializing DCB structure for the write port

    dcbSerialParamsR.DCBlength = sizeof(dcbSerialParamsR);
    dcbSerialParamsW.DCBlength = sizeof(dcbSerialParamsW);

    //retreives  the current settings
    if (
        GetCommState(hCommR, &dcbSerialParamsR) == FALSE ||
        GetCommState(hCommW, &dcbSerialParamsW) == FALSE
    ) { 
        printf("Error >> The GetCommState() function ended with an error\n");
        return finish_program(EXIT_FAILURE);
    }

    dcbSerialParamsR.BaudRate = CBR_115200;  // BaudRate for the read port = 115200
    dcbSerialParamsR.ByteSize = 8;           // ByteSize for the read port = 8
    dcbSerialParamsR.StopBits = ONESTOPBIT;  // StopBits for the read port = 1
    dcbSerialParamsR.Parity   = NOPARITY;    // Parity for the read port   = None

    dcbSerialParamsW.BaudRate = CBR_115200;  // BaudRate for the write port = 115200
    dcbSerialParamsW.ByteSize = 8;           // ByteSize for the write port = 8
    dcbSerialParamsW.StopBits = ONESTOPBIT;  // StopBits for the write port = 1
    dcbSerialParamsW.Parity   = NOPARITY;    // Parity for the write port   = None

    //Configuring the port according to settings in DCB 
    if (
        SetCommState(hCommR, &dcbSerialParamsR) == FALSE ||
        SetCommState(hCommW, &dcbSerialParamsW) == FALSE
    ) {
        printf("Error >> The SetCommState() function ended with an error\n");
        return finish_program(EXIT_FAILURE);
    }
#ifdef _DEBUG
    else {
        // Successfull display the contents of the DCB Structure
        printf("\n\nSetting DCB Structure Successfull:\n\n");
        printf("Baudrate for the read port  = %d\n", dcbSerialParamsR.BaudRate);
        printf("ByteSize for the read port  = %d\n", dcbSerialParamsR.ByteSize);
        printf("StopBits for the read port  = %d\n", dcbSerialParamsR.StopBits);
        printf("Parity for the read port    = %d\n\n", dcbSerialParamsR.Parity);
        printf("Baudrate for the write port = %d\n", dcbSerialParamsW.BaudRate);
        printf("ByteSize for the write port = %d\n", dcbSerialParamsW.ByteSize);
        printf("StopBits for the write port = %d\n", dcbSerialParamsW.StopBits);
        printf("Parity for the write port   = %d\n\n", dcbSerialParamsW.Parity);
    }
#endif // DEBUG

/*----------------------------- Setting Timeouts -----------------------------*/
    COMMTIMEOUTS rTimeouts = { 0 }, wTimeouts = { 0 };

    rTimeouts.ReadIntervalTimeout         = 50;
    rTimeouts.ReadTotalTimeoutConstant    = 50;
    rTimeouts.ReadTotalTimeoutMultiplier  = 10;
    rTimeouts.WriteTotalTimeoutConstant   = 50;
    rTimeouts.WriteTotalTimeoutMultiplier = 10;

    wTimeouts.ReadIntervalTimeout         = 50;
    wTimeouts.ReadTotalTimeoutConstant    = 50;
    wTimeouts.ReadTotalTimeoutMultiplier  = 10;
    wTimeouts.WriteTotalTimeoutConstant   = 50;
    wTimeouts.WriteTotalTimeoutMultiplier = 10;

    if (
        SetCommTimeouts(hCommR, &rTimeouts) == FALSE ||
        SetCommTimeouts(hCommW, &wTimeouts) == FALSE
    ) {
        printf("Error >> The SetCommTimeouts() function ended with an error\n");
        return finish_program(EXIT_FAILURE);
    }
#ifdef _DEBUG
    else printf("Serial Ports Timeout Setting Succeeded\n");
#endif // _DEBUG

/*--------------------------- Setting Receive Mask ---------------------------*/
    //Set up Windows to monitor the serial port for receiving characters.
    if (SetCommMask(hCommR, EV_RXCHAR) == FALSE) {
        printf("Error >> The SetCommMask() function ended with an error\n");
        return finish_program(EXIT_FAILURE);
    }
#ifdef _DEBUG
    else printf("Communication mask port setup successful\n");
#endif // _DEBUG

/*-------------------- Writing a Character to Serial Port --------------------*/
    if (
        WriteFile(
            hCommW,              // Handle to the Serialport
            lpBuffer,            // Data to be written to the port 
            dNumOfBytes2Write,   // Number of bytes to write to the port
            &dNumOfBytesWritten, // Number of bytes written to the port
            NULL
        ) == TRUE
    ) {
        printf("\n%s - Written to %s\n", lpBuffer, wComPortName);
    }
    else {
        printf("\nError >>> %d in Writing to Serial Port\n", GetLastError());
        return finish_program(EXIT_FAILURE);
    }

/*------------------------- Setting WaitComm() Event -------------------------*/
/*----------- Program will Wait here till a Character is received ------------*/
    printf("\n============== Waiting for Data Reception ===============\n\n");
    //Wait for the character to be received
    if (WaitCommEvent(hCommR, &dwEventMask, NULL) == FALSE) {
        printf("Error >> The WaitCommEvent() function ended with an error\n");
        return finish_program(EXIT_FAILURE);
    }
    else {
        //Read the recived data using ReadFile();
        printf("Characters received:\n");
        char TempChar;  // Temporary character
        do {
            ReadFile(hCommR, &TempChar, sizeof(TempChar), &NumBytesRead, NULL);
            SerialBuffer[i] = TempChar;
            i++;
        } while (NumBytesRead > 0);

/*------------------- Printing the recived data to Console --------------------*/
        int j = 0;
        for (j = 0; j < i - 1; j++) {
            printf("%c", SerialBuffer[j]);
        }
    }

/*--------------------------------- Finish -----------------------------------*/
    printf("\n");
    return finish_program(EXIT_SUCCESS);
}
