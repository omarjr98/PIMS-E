#incliude "internal_temp_humidity_h.h"
#include "external_temp_humidity_h.h"
#include "acceleration.h"
#include "calibration.h"
#include <stdio.h>
#include <unistd.h>
#define FILENAME "/mnt/sdcard/sensors.txt"
#define SENSOR_PIN "60"
#define LED_PIN "66"
#include "gpio_control.h"

int main() {
    init_accelerometer();
    export_gpio(SENSOR_PIN);
    export_gpio(LED_PIN);

    float x_offset, y_offset, z_offset;
    calibrate_offsets(&x_offset, &y_offset, &z_offset);

    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Failed to open file for writing data.");
        return 1;
 
    }

    while (1) {
        float external_temperature, external_humidity;
        float internal_temperature, external_humidity;
        
        external_temp_humidity(&external_temperature, &external_humidity);
        internal_temp_humidity(&interanl_temperature, &internal_humidity);

        float x_accel = read_acceleration_x() - x_offset;
        float y_accel = read_acceleration_y() - y_offset;
        float z_accel = read_acceleration_z() - z_offset;

        fprintf(file, "External Temperature: %.2f°C\n", external_temperature);
        fprintf(file, "Internal Humidity: %.2f%%\n", external_humidity);
        fprintf(file, "Temperature: %.2f°C\n", internal_temperature);
        fprintf(file, "Humidity: %.2f%%\n", internal_humidity);
        fprintf(file, "Acceleration in X-Axis: %.2f g\n", x_accel);
        fprintf(file, "Acceleration in Y-Axis: %.2f g\n", y_accel);
        fprintf(file, "Acceleration in Z-Axis: %.2f g\n", z_accel);
       
 char sensor_value = read_value(SENSOR_PIN);

       if (sensor_value != '1')
       {
           int led_fd = open("/sys/class/gpio/gpio66/value", O_WRONLY);
           if (led_fd != -1)
           {
               write(led_fd, "1", 1); // Turn on LED
               close(led_fd);
           }
       }

       usleep(1000); // Check every millisecond
   
    }
   

    fclose(file);
    return 0;
}
