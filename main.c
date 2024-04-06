#include "internal_temp_humidity_h.h"
#include "external_temp_humidity_h.h"
#include "acceleration.h"
#include "calibration.h"
#include "wind_sensor.h"
#include "time.h"
#include <stdio.h>
#include <unistd.h>

#define FILENAME "/mnt/sdcard/sensors3.txt"

int main() {
   
    float x_offset, y_offset, z_offset;
    float x_accel_raw, y_accel_raw, z_accel_raw;

    // Initialize accelerometer and calibrate offsets
    init_accelerometer(&x_accel_raw, &y_accel_raw, &z_accel_raw);
    calibrate_offsets(&x_offset, &y_offset, &z_offset);
    
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Failed to open file for writing data.");
        return 1;
    }

    // Initialize the I2C bus for the wind sensor

    while (1) {
        float external_temperature, external_humidity;
        float internal_temperature, internal_humidity;
        //float wind_speed_mph, Vtemp, TEMP;
       // float x_accel, y_accel, z_accel;

        // Read internal temperature and humidity
        internal_temp_humidity(&internal_temperature, &internal_humidity);

        // Read external temperature and humidity
        external_temp_humidity(&external_temperature, &external_humidity);
        
        x_accel = read_acceleration_x() - x_offset;
        y_accel = read_acceleration_y() - y_offset;
        z_accel = read_acceleration_z() - z_offset;

        

        // Read wind speed and temperature
        //read_wind_speed(&Vtemp, &wind_speed_mph, &TEMP);

        // Print sensor data to console
        printf("Internal Temperature: %.2f°C, Humidity: %.2f%%\n", internal_temperature, internal_humidity);
        
        printf("External Temperature: %.2f°C, Humidity: %.2f%%\n", external_temperature, external_humidity);
        //printf(" VTEMP: %.2f, Wind Speed: %.2f mph, Temperature: %.2f°C \n", Vtemp, wind_speed_mph, TEMP);
        // printf("Acceleration: X: %.2f g, Y: %.2f g, Z: %.2f g\n", x_accel, y_accel, z_accel);

        // Write sensor data to file
       // fprintf(file, "Internal Temperature: %.2f°C, Humidity: %.2f%%\n", internal_temperature, internal_humidity);
        //fprintf(file, "External Temperature: %.2f°C, Humidity: %.2f%%\n", external_temperature, external_humidity);
       // fprintf(file, "VTEMP: %.2f, Wind Speed: %.2f mph, Temperature: %.2f°C\n", Vtemp, wind_speed_mph, TEMP);
        //fprintf(file, "Acceleration: X: %.2f g, Y: %.2f g, Z: %.2f g\n", x_accel, y_accel, z_accel);

        // Delay for 1 second before next reading
        sleep(1);
    }

    fclose(file);
    return 0;
}
