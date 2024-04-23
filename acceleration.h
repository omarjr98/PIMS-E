#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>

#define ADLX345_ADDR 0x53 // I2C address of the ADLX345 accelerometer
#define I2C_BUS 2 // I2C bus number

void read_accelerometer_offsets(int16_t *x_offset, int16_t *y_offset, int16_t *z_offset);
void read_accelerometer_data(int16_t x_offset, int16_t y_offset, int16_t z_offset, int16_t *x, int16_t *y, int16_t *z);

#endif // ACCELEROMETER_H
