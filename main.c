#include "internal_temp_humidity_h.h"
#include "external_temp_humidity_h.h"
#include "accelerometer.h"
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
#define FILENAME "/mnt/sdcard/FinalVersion.txt"



int main() {

	int file;
    int rtc_fd;
    struct rtc_time rtc_tm;


    initialize_accelerometer(&file);
    // Calibrate the accelerometer
    float xOffset, yOffset, zOffset;

    calibrate(file, &xOffset, &yOffset, &zOffset);

    printf("xOffset %.2f , yOffseet %.2f , zOffset %.2f\n", xOffset, yOffset,zOffset);

    float external_temperature, external_humidity;
    float internal_temperature, internal_humidity;
    float xAccl, yAccl, zAccl;
    float Vtemp, wind_speed_mph, TEMP;



    while (1) {



        // Open the RTC device
        rtc_fd = open(RTC_DEVICE, O_RDONLY);
        if (rtc_fd == -1) {
            perror("Failed to open RTC device");
            exit(EXIT_FAILURE);
        }

        // Read the current time from the RTC
        if (ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm) == -1) {
            perror("Failed to read RTC time");
            close(rtc_fd);
            exit(EXIT_FAILURE);
        }

        close(rtc_fd);






        // Read internal temperature and humidity
        internal_temp_humidity(&internal_temperature, &internal_humidity);
    	read_wind_speed(&Vtemp, &wind_speed_mph, &TEMP);
        //Read external temperature and humidity
        external_temp_humidity(&external_temperature, &external_humidity);
        //read_acceleration(file, &xAccl, &yAccl, &zAccl);
    	read_wind_speed(&Vtemp, &wind_speed_mph, &TEMP);
    	read_acceleration(file, &xAccl, &yAccl, &zAccl);

               xAccl -= xOffset;
               yAccl -= yOffset;
               zAccl -= zOffset;

        // Print sensor data to console

        printf("Current date and time: %04d-%02d-%02d %02d:%02d:%02d\n",rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday,rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
        printf("Internal Temperature: %.2fC, Humidity: %.2f% %\n", internal_temperature, internal_humidity);
        printf("External Temperature: %.2fC, Humidity: %.2f% %\n", external_temperature, external_humidity);
        printf("Acceleration in X, Acceleration in Y, Acceleration in Z: %.2f g, %.2f g, %.2f g\n", xAccl, yAccl, zAccl);
        printf("Wind Speed: %.2f\n",wind_speed_mph );



        // Write sensor data to file
        FILE *file = fopen(FILENAME, "a");
        if (file == NULL) {
            perror("Failed to open file for writing data.");
            return 1;
        }

        fprintf(file, "Internal Temperature: %.2fC, Humidity: %.2f%%\n", internal_temperature, internal_humidity);
        fprintf(file, "External Temperature: %.2fC, Humidity: %.2f%%\n", external_temperature, external_humidity);
        fprintf(file, "Acceleration in X, Acceleration in Y, Acceleration in Z: %.2f g, %.2f g, %.2f g\n", xAccl, yAccl, zAccl);
        fprintf(file, "Wind Speed: %.2f\n",wind_speed_mph );
        fclose(file);

        // Delay for 1 second before next reading
        sleep(1);

    }

    return 0;
}


