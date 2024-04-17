#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define ADLX345_ADDR 0x53 // I2C address of the ADLX345 accelerometer
#define I2C_BUS 2 // I2C bus number


int main() {
    int fd;
    char buf[2];
    int16_t x, y, z;
    int16_t x_offset, y_offset, z_offset;
    u_int8_t data_format;
    float x_g, y_g, z_g;

    // Open the I2C device
    char i2c_dev[20];
    sprintf(i2c_dev, "/dev/i2c-%d", I2C_BUS);
    fd = open(i2c_dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }

    // Set the I2C slave address
    if (ioctl(fd, I2C_SLAVE, ADLX345_ADDR) < 0) {
        perror("Failed to set I2C slave address");
        close(fd);
        return 1;
    }

    // Configure the DATA_FORMAT register (0x31) for ±2g range
    buf[0] = 0x31;
    buf[1] = 0x08; // Set the range to ±2g (00b)
    if (write(fd, buf, 2) != 2) {
        perror("Failed to write to I2C device");
        close(fd);
        return 1;
    }

    // Read the DATA_FORMAT register (0x31) to verify the configuration
    buf[0] = 0x31;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return 1;
    }
    if (read(fd, buf, 1) != 1) {
        perror("Failed to read from I2C device");
        close(fd);
        return 1;
    }
    data_format = buf[0];
    printf("DATA_FORMAT (0x31): 0x%02X\n", data_format);

    // Read the offset registers (0x1E, 0x1F, 0x20)
    buf[0] = 0x1E;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return 1;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return 1;
    }
    x_offset = (buf[1] << 8) | buf[0];

    buf[0] = 0x1F;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return 1;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return 1;
    }
    y_offset = (buf[1] << 8) | buf[0];

    buf[0] = 0x20;
    if (write(fd, buf, 1) != 1) {
        perror("Failed to write to I2C device");
        close(fd);
        return 1;
    }
    if (read(fd, buf, 2) != 2) {
        perror("Failed to read from I2C device");
        close(fd);
        return 1;
    }
    z_offset = (buf[1] << 8) | buf[0];

    printf("Offset values: X=%d, Y=%d, Z=%d\n", x_offset, y_offset, z_offset);

    // Read and print the sensor data
    while (1) {
        // Read the DATAX0 and DATAX1 registers (0x32 and 0x33)
        buf[0] = 0x32;
        if (write(fd, buf, 1) != 1) {
            perror("Failed to write to I2C device");
            close(fd);
            return 1;
        }
        if (read(fd, buf, 2) != 2) {
            perror("Failed to read from I2C device");
            close(fd);
            return 1;
        }
        x = (buf[1] << 8) | buf[0];
        x_g = (float)(x - x_offset) * 1 / 256;
        printf("DATAX (0x32-0x33): %.3f g\n", x_g);

        // Read the DATAY0 and DATAY1 registers (0x34 and 0x35)
        buf[0] = 0x34;
        if (write(fd, buf, 1) != 1) {
            perror("Failed to write to I2C device");
            close(fd);
            return 1;
        }
        if (read(fd, buf, 2) != 2) {
            perror("Failed to read from I2C device");
            close(fd);
            return 1;
        }
        y = (buf[1] << 8) | buf[0];
        y_g = (float)(y - y_offset) * 1/ 256;
        printf("DATAY (0x34-0x35): %.3f g\n", y_g);

        // Read the DATAZ0 and DATAZ1 registers (0x36 and 0x37)
        buf[0] = 0x36;
        if (write(fd, buf, 1) != 1) {
            perror("Failed to write to I2C device");
            close(fd);
            return 1;
        }
        if (read(fd, buf, 2) != 2) {
            perror("Failed to read from I2C device");
            close(fd);
            return 1;
        }
        z = (buf[1] << 8) | buf[0];
        z_g = (float)(z - z_offset) * 1/ 256;
        printf("DATAZ (0x36-0x37): %.3f g\n", z_g);

        printf("\n");
        usleep(100000); // Wait for 100 milliseconds
    }

    close(fd);
    return 0;
}
