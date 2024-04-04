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
#define SENSOR_ADDR 0x44

// File path for uSD card
#define FILENAME "/mnt/sdcard/temp_humid.bin"

// Main function
void internal_temp_humidity() {
    int internal_temp_humidity_fd; // Local file descriptor

    // Calls the I2C BUS to open
    if ((internal_temp_humidity_fd = open(I2C_BUS, O_RDWR)) < 0) {
        perror("Failed to open interface bus for internal temperature and humidity sensor");
        exit(1);
    }

    // Set slave address
    if (ioctl(internal_temp_humidity_fd, I2C_SLAVE, SENSOR_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to internal temperature and humidity sensor");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    unsigned char command[] = {0x2C, 0x06}; // Temperature and humidity measurement commands
    if (write(internal_temp_humidity_fd, command, sizeof(command)) != sizeof(command)) {
        perror("Failed to write command to start internal temperature and humidity sensor measurement");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    usleep(20000); // Wait for measurement to complete

    unsigned char buffer[6];
    if (read(internal_temp_humidity_fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
        perror("Failed to read internal temperature and humidity data");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    // Process sensor data and write to file
    float temperature = -45 + 175.0 * ((buffer[0] << 8 | buffer[1]) / 65535.0); // Convert raw temperature to Celsius
    float humidity = 100.0 * ((buffer[3] << 8 | buffer[4]) / 65535.0); // Convert humidity to percentage

    FILE *file = fopen(FILENAME, "a"); // Open file for appending
    if (file == NULL) {
        perror("Failed to open file for writing internal temperature and humidity data");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    // Print on terminal
    printf("Internal Temperature: %.2f degC\n", temperature);
    printf("Internal Humidity: %.2f %%\n", humidity);

    // Write binary data to the file
    char binary_data[64]; // Data size
    int index = 0;

  // Convert temperature and humidity to 32-bit binary with 10 integer bits and 22 fractional bits
  unsigned int temperature_integer = (unsigned int)temperature;
  unsigned int temperature_fractional = (unsigned int)((temperature - temperature_integer) * (1 << 22));
  unsigned int humidity_integer = (unsigned int)humidity;
  unsigned int humidity_fractional = (unsigned int)((humidity - humidity_integer) * (1 << 22));
  unsigned int data[] = {temperature_integer, temperature_fractional, humidity_integer, humidity_fractional};
  fwrite(data, sizeof(unsigned int), sizeof(data) / sizeof(unsigned int), file);
  fprintf(file, "\n");

    fclose(file);
    close(internal_temp_humidity_fd);
}
