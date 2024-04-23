#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "accelerometer.h"

void read_accelerometer_offsets(int16_t *x_offset, int16_t *y_offset, int16_t *z_offset) {
    int fd;
    char buf[2];

    // Open the I2C device
    char i2c_dev[20];
    sprintf(i2c_dev, "/dev/i2c-%d", I2C_BUS);
    fd = open(i2c_dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return;
    }

    // Set the I2C slave address
    if (ioctl(fd, I2C_SLAVE, ADLX345_ADDR) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return;
    }

    // Read the offset registers (0x1E, 0x1F, 0x20)
    buf[0] = 0x1E;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return;
    }
    *x_offset = (buf[1] << 8) | buf[0];

    buf[0] = 0x1F;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return;
    }
    *y_offset = (buf[1] << 8) | buf[0];

    buf[0] = 0x20;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return;
    }
    *z_offset = (buf[1] << 8) | buf[0];

    printf("Offset values: X=%d, Y=%d, Z=%d\n", *x_offset, *y_offset, *z_offset);
    close(fd);
}

void read_accelerometer_data(int16_t x_offset, int16_t y_offset, int16_t z_offset, int16_t *x, int16_t *y, int16_t *z) {
    int fd;
    char buf[2];

    // Open the I2C device
    char i2c_dev[20];
    sprintf(i2c_dev, "/dev/i2c-%d", I2C_BUS);
    fd = open(i2c_dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return;
    }

    // Set the I2C slave address
    if (ioctl(fd, I2C_SLAVE, ADLX345_ADDR) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return;
    }

    // Read the DATAX0 and DATAX1 registers (0x32 and 0x33)
    buf[0] = 0x32;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return;
    }
    *x = (buf[1] << 8) | buf[0];
    *x = *x - x_offset;

    // Read the DATAY0 and DATAY1 registers (0x34 and 0x35)
    buf[0] = 0x34;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return;
    }
    *y = (buf[1] << 8) | buf[0];
    *y = *y - y_offset;

    // Read the DATAZ0 and DATAZ1 registers (0x36 and 0x37)
    buf[0] = 0x36;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return;
    }
    *z = (buf[1] << 8) | buf[0];
    *z = *z - z_offset;

    close(fd);
}
