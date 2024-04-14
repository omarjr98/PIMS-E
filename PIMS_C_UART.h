#include <stdio.h>
#include "SENSOR_REPORT.h"
#include "PIMS_C_SHELL_COMMANDS.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include "internal_temp_humidity.h"
#include "external_temp_humidity.h"
#include "accelerometer.h"
#include "calibration.h"
#include "wind_sensor.h"
#include "time.h"
#include <linux/rtc.h>
#include <sys/ioctl.h>

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
void transmitSensorReport(SensorReport sensorReport) {
	printf("Transmitting UART report...\n");
    // Transmit the Report Transfer Byte
	UART_TX_WRAPPER(SINGLE_REPORT_TRANSFER);

    // Transmit the dateTime
	printf("Transmitting report dateTime: \n");
    unsigned char tempByte;
    for (int i = 0; i < 20; i++) {
        tempByte = sensorReport.dateTime[i];
        printf("%x ", tempByte); // Print each byte before transmission
        UART_TX_WRAPPER(tempByte);
    }
    printf("\n");

    // Transmit the floats from the report, from lsb to msb
    printf("Transmitting report floats...\n");
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
    // txFloat(sensorReport.ultrasound);


    // For debugging
    printf("UART report transmission complete.\n");
    printf("Report DATE: %s\n",sensorReport.dateTime);
    printf("Report x offset: %.2f\n", sensorReport.xOffset);
    printf("Report y offset: %.2f\n", sensorReport.yOffset);
    printf("Report z offset: %.2f\n", sensorReport.zOffset);
    printf("Report x acceleration: %.2f\n", sensorReport.xAccl);
    printf("Report y acceleration: %.2f\n", sensorReport.yAccl);
    printf("Report z acceleration: %.2f\n", sensorReport.zAccl);
    printf("Report temperature: %.2f\n", sensorReport.external_temperature);
    printf("Report humidity: %.2f\n", sensorReport.external_humidity);
    printf("Report temperature: %.2f\n", sensorReport.internal_temperature);
    printf("Report humidity: %.2f\n", sensorReport.internal_humidity);
    printf("Report wind speed: %.2f\n", sensorReport.wind_speed_mph);
}

