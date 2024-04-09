#include "internal_temp_humidity.h"
#include "external_temp_humidity.h"
#include "acceleration.h"
#include "calibration.h"
#include "wind_sensor.h"
#include "time.h"
#include <stdio.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define RTC_DEVICE "/dev/rtc1"
#define FILENAME "/mnt/sdcard/testing.txt"


int main() {

	int file, rtc_fd;
    struct rtc_time rtc_tm;

    // Accelerometer calibration
    initialize_accelerometer(&file);

    float xOffset, yOffset, zOffset;
    float external_temperature, external_humidity;
    float internal_temperature, internal_humidity;
    float xAccl, yAccl, zAccl;
    float Vtemp, wind_speed_mph, TEMP;

    calibrate(file, &xOffset, &yOffset, &zOffset);

    printf("X-Offset %.2f , Y-Offseet %.2f , Z-Offset %.2f\n", xOffset, yOffset,zOffset);


    while (1) {

        // Open RTC
        rtc_fd = open(RTC_DEVICE, O_RDONLY);
        if (rtc_fd == -1) {
            perror("Failed to open RTC device");
            exit(EXIT_FAILURE);
        }

        // Read data from RTC
        if (ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm) == -1) {
            perror("Failed to read RTC time");
            close(rtc_fd);
            exit(EXIT_FAILURE);
        }

        close(rtc_fd);


        // Read internal temperature and humidity
        internal_temp_humidity(&internal_temperature, &internal_humidity);

        //Read external temperature and humidity
        external_temp_humidity(&external_temperature, &external_humidity);

        //Read acceleration
    	read_acceleration(file, &xAccl, &yAccl, &zAccl);
    		//Remove acceleration offsets
               xAccl -= xOffset;
               yAccl -= yOffset;
               zAccl -= zOffset;

        //Read wind speed
    	read_wind_speed(&Vtemp, &wind_speed_mph, &TEMP);


        // Print sensor data to terminal
        printf("%04d-%02d-%02d %02d:%02d:%02d\n",rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday,rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
        printf("Internal Temperature: %.2f C\n, Internal Humidity: %.2f%\n", internal_temperature, internal_humidity);
        printf("External Temperature: %.2f C\n, Internal Humidity: %.2f%\n", external_temperature, external_humidity);
        printf("Acceleration X: %.2f g, Y: %.2f g, Z: %.2f g\n", xAccl, yAccl, zAccl);
        printf("Wind Speed: %.2f MPH\n",wind_speed_mph );


        // Write sensor data to memory bank
        FILE *file = fopen(FILENAME, "a");
        if (file == NULL) {
            perror("Failed to open file for writing data to SD Card.");
            return 1;
        }

        fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d",rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday,rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
        fprintf(file, "%.2fC %.2f%", internal_temperature, internal_humidity);
        fprintf(file, "%.2fC %.2f%", external_temperature, external_humidity);
        fprintf(file, "X:%.2fg, Y:%.2f g, Z:%.2f g", xAccl, yAccl, zAccl);
        fprintf(file, "%.2fMPH\n",wind_speed_mph );
        fclose(file);

        // Delay for 1 second before next reading
        sleep(1);

    }

    return 0;
}
