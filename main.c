#include "internal_temp_humidity_h.h"
#include "external_temp_humidity_h.h"
//#include "wind_sensor.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define FILENAME "/mnt/sdcard/sensors3.txt"

int main() {
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

        // Read internal temperature and humidity
        internal_temp_humidity(&internal_temperature, &internal_humidity);

        // Read external temperature and humidity
        external_temp_humidity(&external_temperature, &external_humidity);

        // Read wind speed and temperature
        //read_wind_speed(&Vtemp, &wind_speed_mph, &TEMP);

        // Print sensor data to console
        printf("Internal Temperature: %.2f°C, Humidity: %.2f%%\n", internal_temperature, internal_humidity);
        printf("External Temperature: %.2f°C, Humidity: %.2f%%\n", external_temperature, external_humidity);
        //printf(" VTEMP: %.2f, Wind Speed: %.2f mph, Temperature: %.2f°C \n", Vtemp, wind_speed_mph, TEMP);

        // Write sensor data to file
       // fprintf(file, "Internal Temperature: %.2f°C, Humidity: %.2f%%\n", internal_temperature, internal_humidity);
        //fprintf(file, "External Temperature: %.2f°C, Humidity: %.2f%%\n", external_temperature, external_humidity);
       // fprintf(file, "VTEMP: %.2f, Wind Speed: %.2f mph, Temperature: %.2f°C\n", Vtemp, wind_speed_mph, TEMP);

        // Delay for 1 second before next reading
        sleep(1);
    }

    fclose(file);
    return 0;
}
