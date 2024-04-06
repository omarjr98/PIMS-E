#ifndef ACCELERATION_H
#define ACCELERATION_H

#include <stdint.h> // Include for int16_t type

void init_accelerometer(float *x_accel_raw , float *y_accel_raw , float *z_accel_raw);
float read_acceleration_x();
float read_acceleration_y();
float read_acceleration_z();
int16_t read_signed16(int reg); // Add function prototype here

#endif // ACCELERATION_H
