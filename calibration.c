#include "calibration.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CALIBRATION_SAMPLES 100


void calibrate(int file, float *xOffset,float *yOffset,float *zOffset) {
    int i;
    int xSum = 0, ySum = 0, zSum = 0;

    for (i = 0; i < CALIBRATION_SAMPLES; i++) {
        // Read 6 bytes of data from register(0x32)
        // xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
        char reg = 0x32;
        if (write(file, &reg, 1) != 1) {
            printf("Error writing to register.\n");
            exit(1);
        }

        char data[6] = {0};
        if (read(file, data, 6) != 6) {
            printf("Error reading from register.\n");
            exit(1);
        }

        // Sum up the raw data for each axis
        xSum += ((data[1] & 0x03) * 256 + (data[0] & 0xFF));
        ySum += ((data[3] & 0x03) * 256 + (data[2] & 0xFF));
        zSum += ((data[5] & 0x03) * 256 + (data[4] & 0xFF));

        // Delay before the next reading
        usleep(10000); // 10ms delay
    }

    // Calculate average offsets
    *xOffset = xSum / CALIBRATION_SAMPLES;
    *yOffset = ySum / CALIBRATION_SAMPLES;
    *zOffset = zSum / CALIBRATION_SAMPLES;
}

