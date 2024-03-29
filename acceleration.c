#include "acceleration.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS "/dev/i2c-1"
#define ADXL345_ADDR 0x53
#define ADXL345_DATA_X0 0x32
#define ADXL345_DATA_Y0 0x34
#define ADXL345_DATA_Z0 0x36

static int accelerometer = -1; // File descriptor for the accelerometer device

void init_accelerometer() {
    if ((accelerometer = open(I2C_BUS, O_RDWR)) < 0) {
        perror("Failed to open accelerometer bus.");
        exit(1);
    }
    if (ioctl(accelerometer, I2C_SLAVE, ADXL345_ADDR) < 0) {
        perror("Failed to connect to the accelerometer.");
        exit(1);
    }
}

float read_acceleration_x() {
    int16_t x_accel_raw = read_signed16(ADXL345_DATA_X0);
    return (float)(x_accel_raw * 3.9 / 1000.0);
}

float read_acceleration_y() {
    int16_t y_accel_raw = read_signed16(ADXL345_DATA_Y0);
    return (float)(y_accel_raw * 3.9 / 1000.0);
}

float read_acceleration_z() {
    int16_t z_accel_raw = read_signed16(ADXL345_DATA_Z0);
    return (float)(z_accel_raw * 3.9 / 1000.0);
}

int16_t read_signed16(int reg) { // Function definition
    uint8_t low, high;
    int16_t value;

    if (write(accelerometer, &reg, 1) != 1) {
        perror("Failed to select register for accelerometer.");
        exit(1);
    }
    if (read(accelerometer, &low, 1) != 1 || read(accelerometer, &high, 1) != 1) {
        perror("Failed to read data for accelerometer.");
        exit(1);
    }

    value = (high << 8) | low;
    if (value & (1 << 15)) {  // Check sign bit
        value -= 1 << 16;
    }
    return value;
}
