#include <stdio.h>
#include "SENSOR_REPORT.h"
#include "PIMS_C_SHELL_COMMANDS.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#define UART_DEVICE "/dev/sr1"

// Remove at some point
// To receive data, connect the P9_26 (UART1_RXD) pin on the BeagleBone Black 
// To transmit data, connect the P9_24 (UART1_TXD) pin on the BeagleBone Black

unsigned char mockRXBuffer[SENSOR_REPORT_SIZE];
int mockRXCounter = 0;
void MOCK_UART_TX(unsigned char inputByte){
    printf("%x ", inputByte);
    mockRXBuffer[mockRXCounter] = inputByte;
    mockRXCounter++;
    if (mockRXCounter > SENSOR_REPORT_SIZE){
        mockRXCounter = 0;
    }
}


//Read in the bytes until the UART Message is complete
//Construct an input to the shell from the UART Message bytes

//Instruction byte will be first, followed by data bytes

//To send a Sensor report, the sensor report struct has to be created
//on the PIMS E side. Then the method can be called to transmit the 
//Sensor report in a pre-determeined way:
void UART_TX_WRAPPER(unsigned char inputByte){
    //Place Platform Specific UART T_X Function for platform here:
  
    //Open UART
    int uart_fd;
    uart_fd = open(UART_DEVICE, O_WRONLY | O_NOCTTY);
    if (uart_fd == -1) {
        perror("Failed to open UART device");
        return 1;
    }
  
    // Configure UART settings
    struct termios options;
    tcgetattr(uart_fd, &options);
    cfsetospeed(&options, B9600);  // Set baud rate
    options.c_cflag &= ~PARENB;    // Disable parity bit
    options.c_cflag &= ~CSTOPB;    // Set one stop bit`
    options.c_cflag &= ~CSIZE;     // Clear character size mask
    options.c_cflag |= CS8;        // Set 8 bits per character
    options.c_cflag &= ~CRTSCTS;   // Disable hardware flow control
    options.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem status lines
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Set raw mode
    tcsetattr(uart_fd, TCSANOW, &options);

    // Transmit data
    const char *data = "Hello, UART!\n";
    ssize_t bytes_written = write(uart_fd, data, strlen(data));
    if (bytes_written == -1) {
        perror("Failed to write to UART device");
        close(uart_fd);
        return 1;
    }

    // Close UART device
    close(uart_fd);

    return 0;
    }
  
    //Code below is for testing
    MOCK_UART_TX(inputByte);

}

void txFloat(float input)
{
    //Cast the address of the float to
    //the address of a character array
    unsigned char *p = (unsigned char*)&input;

    //Loop through array and transmit each byte
    for (int i = 0; i < 4; i++){
        UART_TX_WRAPPER((unsigned char)p[i]);
    }
}

//Read in the bytes until the UART Message is complete
//Construct an input to the shell from the UART Message bytes

//Instruction byte will be first, followed by data bytes

//To send a Sensor report, the sensor report struct has to be created
//on the PIMS E side. Then the method can be called to transmit the 
//Sensor report in a pre-determeined way:
void transmitSensorReport(SensorReport sensorReport){
  
    //Transmit the Report Transfer Byte
    UART_TX_WRAPPER(SINGLE_REPORT_TRANSFER);

    //Transmit the dateTime
    unsigned char tempByte;
    for (int i = 0; i < 20; i++){
        tempByte = sensorReport.dateTime[i];
        UART_TX_WRAPPER(tempByte);
    }

    //Transmit the floats from the report, from lsb to msb

    txFloat(sensorReport.xOffset);
    txFloat(sensorReport.yOffset);
    txFloat(sensorReport.zOffset);
    txFloat(sensorReport.xAccl);
    txFloat(sensorReport.yAccl);
    txFloat(sensorReport.zAccl);
    txFloat(sensorReport.external_temperature);
    txFloat(sensorReport.external_humidity);
    txFloat(sensorReport.internal_temperature);
    txFloat(sensorReport.internal_humidity);
    txFloat(sensorReport.wind_speed_mph);
    //txFloat(sensorReport.float ultrasound);



}

