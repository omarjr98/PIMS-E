#incliude "internal_temp_humidity_h.h"
#include "external_temp_humidity_h.h"
#include "acceleration.h"
#include "calibration.h"
#include <stdio.h>
#include <unistd.h>
#define FILENAME "/mnt/sdcard/sensors.txt"

int main() {
    init_accelerometer();

    float x_offset, y_offset, z_offset;
    calibrate_offsets(&x_offset, &y_offset, &z_offset);

    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Failed to open file for writing data.");
        return 1;
    }

    while (1) {
        float temperature, humidity;
        external_temp_humidity(&temperature, &humidity);
        internal_temp_humidity(&temperature, &humidity);

        float x_accel = read_acceleration_x() - x_offset;
        float y_accel = read_acceleration_y() - y_offset;
        float z_accel = read_acceleration_z() - z_offset;

        fprintf(file, "Temperature: %.2f°C\n", temperature);
        fprintf(file, "Humidity: %.2f%%\n", humidity);
        fprintf(file, "Acceleration in X-Axis: %.2f g\n", x_accel);
        fprintf(file, "Acceleration in Y-Axis: %.2f g\n", y_accel);
        fprintf(file, "Acceleration in Z-Axis: %.2f g\n", z_accel);

        usleep(100000);  // Delay for 100 milliseconds
    }

    fclose(file);
    return 0;
}
