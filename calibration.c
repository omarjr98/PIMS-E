#include "calibration.h"
#include "acceleration.h"
#include <stdio.h>
#include <unistd.h>

void calibrate_offsets(float *x_offset, float *y_offset, float *z_offset) {
    printf("Starting calibration...\n");

    // Accumulators for calibration samples
    float x_accumulator = 0;
    float y_accumulator = 0;
    float z_accumulator = 0;

    // Collect samples for calibration
    int i;
    for (i = 0; i < CALIBRATION_SAMPLES; ++i) {
        x_accumulator += read_acceleration_x();
        y_accumulator += read_acceleration_y();
        z_accumulator += read_acceleration_z();
        usleep(10000);  // Delay between samples (10ms)
    }

    // Calculate average offsets
    *x_offset = x_accumulator / CALIBRATION_SAMPLES;
    *y_offset = y_accumulator / CALIBRATION_SAMPLES;
    *z_offset = z_accumulator / CALIBRATION_SAMPLES;

    printf("Calibration complete.\n");
    printf("Offset in X-Axis: %.2f\n", *x_offset);
    printf("Offset in Y-Axis: %.2f\n", *y_offset);
    printf("Offset in Z-Axis: %.2f\n", *z_offset);
}
