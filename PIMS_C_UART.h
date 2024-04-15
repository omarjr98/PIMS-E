#include <stdio.h>
#include "SENSOR_REPORT.h"
#include "PIMS_C_SHELL_COMMANDS.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#define UART_DEVICE "/dev/ttyS1"

//Read in the bytes until the UART Message is complete
//Construct an input to the shell from the UART Message bytes

//Instruction byte will be first, followed by data bytes

//To send a Sensor report, the sensor report struct has to be created
//on the PIMS E side. Then the method can be called to transmit the
//Sensor report in a pre-determeined way:
void UART_TX_WRAPPER(unsigned char inputByte){
//Place Platform Specific UART T_X Function for platform here:

    // Open UART
    int uart_fd;
    struct termios uart_options;
    uart_fd = open(UART_DEVICE, O_WRONLY | O_NOCTTY);
    if (uart_fd == -1) {
        perror("Failed to open UART device");
        return;
    }

    // Get current UART settings
        tcgetattr(uart_fd, &uart_options);

        // Set baud rate
        cfsetospeed(&uart_options, B115200); // TX desired baud rate
        cfsetispeed(&uart_options, B115200); // RX desired baud rate

        // Apply UART settings
        tcsetattr(uart_fd, TCSANOW, &uart_options);


    // Transmit data
    ssize_t bytes_written = write(uart_fd, &inputByte, 1);
    if (bytes_written == -1) {
        perror("Failed to write to UART device");
    }
    	printf("%x ",inputByte);

    // Close UART device
    close(uart_fd);
}

void txFloat(float input)
{
    //Cast the address of the float to
    //the address of a character array
    unsigned char *p = (unsigned char*)&input;

    //Loop through array and transmit each byte
    for (int i = 0; i < 4; i++) {
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
    int counter = 0;
    UART_TX_WRAPPER(SINGLE_REPORT_TRANSFER);
    counter ++;
    //Transmit the dateTime
    unsigned char tempByte;
    for (int i = 0; i < 32; i++){
        tempByte = sensorReport.dateTime[i];
        UART_TX_WRAPPER(tempByte);
        counter ++;
    }


    //Transmit the floats from the report, from lsb to msb

    txFloat(sensorReport.temperatureExternal);
    counter +=4;
    txFloat(sensorReport.humidityExternal);
    counter +=4;
    txFloat(sensorReport.temperatureInternal);
    counter +=4;
    txFloat(sensorReport.humidityInternal);
    counter +=4;
    txFloat(sensorReport.accelXMin);
    counter +=4;
    txFloat(sensorReport.accelXMax);
    counter +=4;
    txFloat(sensorReport.accelYMin);
    counter +=4;
    txFloat(sensorReport.accelYMax);
    counter +=4;
    txFloat(sensorReport.accelZMin);
    counter +=4;
    txFloat(sensorReport.accelZMax);
    counter +=4;
    txFloat(sensorReport.accelXOffset);
    counter +=4;
    txFloat(sensorReport.accelYOffset);
    counter +=4;
    txFloat(sensorReport.accelZOffset);
    counter +=4;
    txFloat(sensorReport.windSpeed);
    counter +=4;
    txFloat(sensorReport.ultrasound);
    counter +=4;

    UART_TX_WRAPPER(0xDD);
    UART_TX_WRAPPER(0xDD);


    // For debugging
    printf("\n");
    printf("-------------- UART Report Transmitted --------------\n");
    printf( " Date: %s\n",sensorReport.dateTime);
    printf( " External Temperature: %.2f\n", sensorReport.temperatureExternal);
    printf( " External Humidity: %.2f\n", sensorReport.humidityExternal);
    printf( " Internal Temperature: %.2f\n", sensorReport.temperatureInternal);
    printf( " Internal Humidity: %.2f\n", sensorReport.humidityInternal);
    printf( " X_Min: %.2f\n", sensorReport.accelXMin);
    printf( " X_Max: %.2f\n", sensorReport.accelXMax);
    printf( " Y_Min: %.2f\n", sensorReport.accelYMin);
    printf( " Y_Max: %.2f\n", sensorReport.accelYMax);
    printf( " Z_Min: %.2f\n", sensorReport.accelZMin);
    printf( " Z_Max: %.2f\n", sensorReport.accelZMax);
    printf( " X_Offset: %.2f\n", sensorReport.accelXOffset);
    printf( " Y_Offset: %.2f\n", sensorReport.accelYOffset);
    printf( " Z_Offset: %.2f\n", sensorReport.accelZOffset);
    printf( " Wind speed: %.2f\n", sensorReport.windSpeed);
    printf( " Ultrasound: %.2f\n", sensorReport.ultrasound);
    printf("----------------- UART Report END --------------------\n");
}
