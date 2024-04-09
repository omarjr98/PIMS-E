#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>
#include "external_temp_humidity_h.h"

// I2C BUS #2
#define I2C_BUS "/dev/i2c-2"

// Address of the SHT31 sensor
#define SENSOR_ADDR 0x44

// File path for uSD card


// Main function
void external_temp_humidity(float *external_temperature, float *external_humidity) {
    int external_temp_humidity_fd; // Local file descriptor

    // Calls the I2C BUS to open
    if ((external_temp_humidity_fd = open(I2C_BUS, O_RDWR)) < 0) {
        perror("Failed to open interface bus for external temperature and humidity sensor");
        exit(1);
    }

    // Set slave address
    if (ioctl(external_temp_humidity_fd, I2C_SLAVE, SENSOR_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to external temperature and humidity sensor");
        close(external_temp_humidity_fd);
        exit(1);
    }

    unsigned char command[] = {0x2C, 0x06}; // Temperature and humidity measurement commands
    if (write(external_temp_humidity_fd, command, sizeof(command)) != sizeof(command)) {
        perror("Failed to write command to start external temperature and humidity sensor measurement");
        close(external_temp_humidity_fd);
        exit(1);
    }

    usleep(20000); // Wait for measurement to complete

    unsigned char buffer[6];
    if (read(external_temp_humidity_fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
        perror("Failed to read external temperature and humidity data");
        close(external_temp_humidity_fd);
        exit(1);
    }

    // Process sensor data and write to file
     *external_temperature = -45 + 175.0 * ((buffer[0] << 8 | buffer[1]) / 65535.0); // Convert raw temperature to Celsius
     *external_humidity = 100.0 * ((buffer[3] << 8 | buffer[4]) / 65535.0); // Convert humidity to percentage

  // Print on terminal
 // printf("External Temperature: %.2f degC\n", external_temperature);
  //printf("External Humidity: %.2f %%\n", external_humidity);





    close(external_temp_humidity_fd);
}
