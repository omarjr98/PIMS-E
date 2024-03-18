#include "acceleration.h"
#include "calibration.h"
#include <stdio.h>
#include <unistd.h>

#define FILENAME "/mnt/sdcard/accelerometer.txt"

int main() {
    // Initialize accelerometer
    init_accelerometer();

    // Calibrate accelerometer
    float x_offset, y_offset, z_offset;
    calibrate_offsets(&x_offset, &y_offset, &z_offset);

    // Open file for appending
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Failed to open file for writing acceleration data.");
        return 1;
    }

    // Main loop to continuously read acceleration data
    while (1) {
        // Read acceleration values from each axis
        float x_accel = read_acceleration_x() - x_offset;
        float y_accel = read_acceleration_y() - y_offset;
        float z_accel = read_acceleration_z() - z_offset;

        // Save acceleration data to file
        fprintf(file, "Acceleration in X-Axis: %.2f g\n", x_accel);
        fprintf(file, "Acceleration in Y-Axis: %.2f g\n", y_accel);
        fprintf(file, "Acceleration in Z-Axis: %.2f g\n", z_accel);

        // Sleep for some time before the next reading
        sleep(1);
    }

    // Close the file
    fclose(file);

    return 0;
}
