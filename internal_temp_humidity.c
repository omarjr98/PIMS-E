#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>
#include "internal_temp_humidity_h.h"

// I2C BUS #2
#define I2C_BUS "/dev/i2c-2"

// Address of the SHT31 sensor
#define SENSOR_ADDR 0x45

// File path for uSD card
#define FILENAME "/mnt/sdcard/temp_humid.bin"

// Function to open the I2C bus
int open_i2c_bus() {
    int i2c_fd;
    if ((i2c_fd = open(I2C_BUS, O_RDWR)) < 0) {
        perror("Failed to open interface bus for internal temperature and humidity sensor");
        exit(1);
    }
    return i2c_fd;
}

// Function to close the I2C bus
void close_i2c_bus(int i2c_fd) {
    if (close(i2c_fd) < 0) {
        perror("Failed to close interface bus for internal temperature and humidity sensor");
        exit(1);
    }
}

// Main function
void internal_temp_humidity(float *internal_temperature, float *internal_humidity) {
    int internal_temp_humidity_fd; // Local file descriptor

    // Open the I2C bus
    internal_temp_humidity_fd = open_i2c_bus();

    // Set slave address
    if (ioctl(internal_temp_humidity_fd, I2C_SLAVE, SENSOR_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to internal temperature and humidity sensor");
        close_i2c_bus(internal_temp_humidity_fd);
        exit(1);
    }

    unsigned char command[] = {0x2C, 0x06}; // Temperature and humidity measurement commands
    if (write(internal_temp_humidity_fd, command, sizeof(command)) != sizeof(command)) {
        perror("Failed to write command to start internal temperature and humidity sensor measurement");
        close_i2c_bus(internal_temp_humidity_fd);
        exit(1);
    }

    usleep(20000); // Wait for measurement to complete

    unsigned char buffer[6];
    if (read(internal_temp_humidity_fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
        perror("Failed to read internal temperature and humidity data");
        close_i2c_bus(internal_temp_humidity_fd);
        exit(1);
    }

    // Process sensor data and write to file
    *internal_temperature = -45 + 175.0 * ((buffer[0] << 8 | buffer[1]) / 65535.0); // Convert raw temperature to Celsius
    *internal_humidity = 100.0 * ((buffer[3] << 8 | buffer[4]) / 65535.0); // Convert humidity to percentage

    FILE *file = fopen(FILENAME, "a"); // Open file for appending
    if (file == NULL) {
        perror("Failed to open file for writing internal temperature and humidity data");
        close_i2c_bus(internal_temp_humidity_fd);
        exit(1);
    }

    fclose(file);

    // Close the I2C bus
    close_i2c_bus(internal_temp_humidity_fd);
}
