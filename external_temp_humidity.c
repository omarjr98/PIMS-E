#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>
#include "internal_temp_humidity.h"

#define I2C_BUS "/dev/i2c-2"
#define SENSOR_ADDR 0x45


void internal_temp_humidity(float *internal_temperature, float *internal_humidity) {
    int internal_temp_humidity_fd;

    // Open I2C_BUS_2
    if ((internal_temp_humidity_fd = open(I2C_BUS, O_RDWR)) < 0) {
        perror("Failed to open I2C BUS 2 for internal temperature and humidity sensor");
        exit(1);
    }

    // Slave SHT31 address
    if (ioctl(internal_temp_humidity_fd, I2C_SLAVE, SENSOR_ADDR) < 0) {
        perror("Failed to acquire I2C BUS 2 access and/or talk to internal temperature and humidity sensor");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    unsigned char command[] = {0x2C, 0x06}; // Temperature and humidity measurement commands
    if (write(internal_temp_humidity_fd, command, sizeof(command)) != sizeof(command)) {
        perror("Failed to write command to start internal temperature and humidity sensor measurement");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    usleep(20000);

    unsigned char buffer[6];
    if (read(internal_temp_humidity_fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
        perror("Failed to read internal temperature and humidity data");
        close(internal_temp_humidity_fd);
        exit(1);
    }

    // Process sensor data
     *internal_temperature = -45 + 175.0 * ((buffer[0] << 8 | buffer[1]) / 65535.0); // Convert raw temperature to Celsius
     *internal_humidity = 100.0 * ((buffer[3] << 8 | buffer[4]) / 65535.0); // Convert humidity to percentage

    close(internal_temp_humidity_fd);
}
