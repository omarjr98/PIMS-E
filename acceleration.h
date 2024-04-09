#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H


void initialize_accelerometer(int *file);
void read_acceleration(float file, float *xAccl, float *yAccl, float *zAccl);

#endif
