#include "internal_temp_humidity.h"
#include "external_temp_humidity.h"
#include "accelerometer.h"
#include "wind_sensor.h"
#include "door_sensor.h"
#include "time.h"
#include "PIMS_C_UART.h"
#include <stdio.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>

#define RTC_DEVICE "/dev/rtc1"
#define UART_DEVICE "/dev/ttyS1"
#define FILENAME "/mnt/sdcard/testing.txt"

int main() {
    int rtc_fd;
    struct rtc_time rtc_tm;
    char dateTime[20];

    // Accelerometer initialization and calibration


    int16_t x_offset, y_offset, z_offset;
       int16_t x, y, z;
       float x_g, y_g, z_g;

    float external_temperature, external_humidity;
    float internal_temperature, internal_humidity;

    float Vtemp, wind_speed_mph, TEMP;
    read_accelerometer_offsets(&x_offset, &y_offset, &z_offset);
    printf(" X_o:%.2fg\n Y_o:%.2fg\n Z_o:%.2fg\n", x_offset, y_offset, z_offset);
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
        }

        close(rtc_fd);

        // Get current date and time
        snprintf(dateTime, 20, "%02d/%02d/%04d %02d:%02d:%02d",
                 rtc_tm.tm_mon + 1, rtc_tm.tm_mday, rtc_tm.tm_year + 1900,
                 rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

        // Read external temperature and humidity
        external_temp_humidity(&external_temperature, &external_humidity);

        // Read acceleration
        read_accelerometer_data(x_offset, y_offset, z_offset, &x, &y, &z);
        x_g = (float)x / 256.0f;
        y_g = (float)y / 256.0f;
        z_g = (float)z / 256.0f;
        // Remove acceleration offsets


        // Read wind speed
        read_wind_speed(&Vtemp, &wind_speed_mph, &TEMP);

        // Door sensor
        door_sensor_main();

        // Print sensor data to terminal
        printf("-------------- PIMS-E Memory Report --------------\n");
        printf(" %s UTC\n", dateTime);

        printf(" X:%.2fg\n Y:%.2fg\n Z:%.2fg\n", x_g, y_g,z_g);
        printf(" X_l:nan\n Y_l:nan\n Z_l:nan\n"); // Acceleration Min
        printf(" T_e:%.2fC\n H_e:%.2f%%\n", external_temperature, external_humidity);
        printf(" T_i:%.2fC\n H_i:%.2f%%\n", internal_temperature, internal_humidity);
        printf(" W:%.2fMPH\n", wind_speed_mph);
        printf(" U:nan\n"); // Ultrasound
        printf("-------------- PIMS-E Memory Report END --------------\n");

        // Write sensor data to memory bank
        FILE *outputFile = fopen(FILENAME, "a");
        if (outputFile == NULL) {
            perror("Failed to open file for writing data to SD Card.");
            return 1;
        }

        fprintf(outputFile, " %s UTC", dateTime);
        fprintf(outputFile, " X_o:%.2fg Y_o:%.2fg Z_o:%.2fg",x_offset, y_offset, z_offset );
        fprintf(outputFile, " X_h:%.2fg Y_h:%.2fg Z_h:%.2fg", x_g, y_g,z_g);
        fprintf(outputFile, " X_l:nan Y_l:nan Z_l:nanf"); // Acceleration Min
        fprintf(outputFile, " T_e:%.2fC H_e:%.2f%%", external_temperature, external_humidity);
        fprintf(outputFile, " T_i:%.2fC H_i:%.2f%%", internal_temperature, internal_humidity);
        fprintf(outputFile, " W:%.2fMPH", wind_speed_mph);
        fprintf(outputFile, " U:nan\n"); // Ultrasound
        fclose(outputFile);

        // PIMS-C UART
        SensorReport ttyS1 = {0};
        strcpy(ttyS1.dateTime, dateTime);
        ttyS1.accelXOffset = x_offset;
        ttyS1.accelYOffset = y_offset;
        ttyS1.accelZOffset = z_offset;
        ttyS1.accelXMax = x_g;
        ttyS1.accelXMin = NAN;
        ttyS1.accelYMax = y_g;
        ttyS1.accelYMin = NAN;
        ttyS1.accelZMax = z_g;
        ttyS1.accelZMin = NAN;
        ttyS1.temperatureExternal = external_temperature;
        ttyS1.humidityExternal = external_humidity;
        ttyS1.temperatureInternal = internal_temperature;
        ttyS1.humidityInternal = internal_humidity;
        ttyS1.windSpeed = wind_speed_mph;
        ttyS1.ultrasound = NAN;

        transmitSensorReport(ttyS1);

        // Delay for 1 second before next reading
        sleep(1);
    }




    return 1;
}

