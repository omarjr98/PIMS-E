#include "internal_temp_humidity_h.h"
#include "external_temp_humidity_h.h"
#include "accelerometer.h"
#include "calibration.h"
#include "wind_sensor.h"
#include "time.h"
#include <stdio.h>
#include <unistd.h>

#define FILENAME "/mnt/sdcard/sensors3.txt"

int main() {
    int file;
    initialize_accelerometer(&file);
    float external_temperature, external_humidity;
    float internal_temperature, internal_humidity;
    float xAccl, yAccl, zAccl;
    float xOffset, yOffset, zOffset;

    // Calibrate the accelerometer
    calibrate(file, &xOffset, &yOffset, &zOffset);

    while (1) {
        // Read internal temperature and humidity
        internal_temp_humidity(&internal_temperature, &internal_humidity);

        // Read external temperature and humidity
        external_temp_humidity(&external_temperature, &external_humidity);
        read_acceleration(file, &xAccl, &yAccl, &zAccl);

        // Apply the calibration offsets
        xAccl -= xOffset;
        yAccl -= yOffset;
        zAccl -= zOffset;

        // Print sensor data to console
        printf("Internal Temperature: %.2f degrees C, Humidity: %.2f% %\n", internal_temperature, internal_humidity);
        printf("External Temperature: %.2f degrees C, Humidity: %.2f% %\n", external_temperature, external_humidity);
        printf("Acceleration in X, Acceleration in Y, Acceleration in Z: %.2f g, %.2f g, %.2f g\n", xAccl, yAccl, zAccl);

        // Write sensor data to file
        FILE *file = fopen(FILENAME, "a");
        if (file == NULL) {
            perror("Failed to open file for writing data.");
            return 1;
        }

        fprintf(file, "Internal Temperature: %.2f°C, Humidity: %.2f%%\n", internal_temperature, internal_humidity);
        fprintf(file, "External Temperature: %.2f°C, Humidity: %.2f%%\n", external_temperature, external_humidity);
        fprintf(file, "Acceleration in X, Acceleration in Y, Acceleration in Z: %.2f g, %.2f g, %.2f g\n", xAccl, yAccl, zAccl);

        fclose(file);

        // Delay for 1 second before next reading
        sleep(1);
    }

    return 0;
}

