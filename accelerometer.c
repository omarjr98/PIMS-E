#include "accelerometer.h"
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

void initialize_accelerometer(int *file) {
    // Open I2C_BUS_2
    char *bus = "/dev/i2c-2";
    if ((*file = open(bus, O_RDWR)) < 0) {
        printf("Failed to open I2C BUS 2 for accelerometer.\n");
        exit(1);
    }

    // Slave ADXL345 address
    if (ioctl(*file, I2C_SLAVE, 0x53) < 0) {
        printf("Failed to acquire I2C BUS access and/or talk to accelerometer.\n");
        exit(1);
    }

    // Select Bandwidth rate register(0x2C)
    // Normal mode, Output data rate = 100 Hz(0x0A)
    char config[2] = {0x2C, 0x0A};
    write(*file, config, 2);

    // Select Power control register(0x2D)
    // Auto-sleep disable(0x08)
    config[0] = 0x2D;
    config[1] = 0x08;
    write(*file, config, 2);

    // Select Data format register(0x31)
    // Self test disabled, 4-wire interface, Full resolution, range = +/-2g(0x08)
    config[0] = 0x31;
    config[1] = 0x08;
    write(*file, config, 2);
    sleep(1);
}

void read_acceleration(float file, float *xAccl, float *yAccl, float *zAccl) {
    // Read 6 bytes of data from register(0x32)
    // xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
    char reg = 0x32;
    if (write(file, &reg, 1) != 1) {
        printf("Error writing to accelerometer registers.\n");
        exit(1);
    }

    char data[6] = {0};
    if (read(file, data, 6) != 6) {
        printf("Error reading from accelerometer registers.\n");
        exit(1);
    }

    // Convert the data to 10-bits
    *xAccl = ((data[1] & 0x03) * 256 + (data[0] & 0xFF));
    if (*xAccl > 511) {
        *xAccl -= 1024;
    }

    *yAccl = ((data[3] & 0x03) * 256 + (data[2] & 0xFF));
    if (*yAccl > 511) {
        *yAccl -= 1024;
    }

    *zAccl = ((data[5] & 0x03) * 256 + (data[4] & 0xFF));
    if (*zAccl > 511) {
        *zAccl -= 1024;
    }
}

