#include "internal_temp_humidity.h"
#include "external_temp_humidity.h"
#include "accelerometer.h"
#include "calibration.h"
#include "wind_sensor.h"
#include "door_sensor.h"
#include "time.h"
#include <stdio.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "PIMS_C_UART.h"
#define RTC_DEVICE "/dev/rtc0"
#define UART_DEVICE "/dev/ttyS1"



#define FILENAME "/mnt/sdcard/testing.txt"

int main() {

   int file, rtc_fd;
   struct rtc_time rtc_tm;
   char dateTime[20];

  //accelerometer calibration
    initialize_accelerometer(&file);

    float xOffset, yOffset, zOffset;
    float external_temperature, external_humidity;
    float internal_temperature, internal_humidity;
    float xAccl, yAccl, zAccl;
    float Vtemp, wind_speed_mph, TEMP;

    calibrate(file, &xOffset, &yOffset, &zOffset);

    printf("X-Offset %.2f , Y-Offseet %.2f , Z-Offset %.2f\n", xOffset, yOffset,zOffset);


    while (1) {
      //Read door status
    	door_sensor_main();

       //Open RTC
        rtc_fd = open(RTC_DEVICE, O_RDONLY);
        if (rtc_fd == -1) {
            perror("Failed to open RTC device");
            exit(EXIT_FAILURE);
        }

    //   Read data from RTC
        if (ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm) == -1) {
            perror("Failed to read RTC time");
            close(rtc_fd);
            //exit(EXIT_FAILURE);
        }

     close(rtc_fd);

        // Get current date and time
      snprintf(dateTime, 20, "%02d/%02d/%04d %02d:%02d:%02d",
    		  	 rtc_tm.tm_mon + 1, rtc_tm.tm_mday, rtc_tm.tm_year + 1900,
                 rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

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
        printf("%s\n", dateTime);
        printf("Internal Temperature: %.2fC\nInternal Humidity: %.2f%\n", internal_temperature, internal_humidity);
        printf("External Temperature: %.2fC\nInternal Humidity: %.2f%\n", external_temperature, external_humidity);
        printf("Acceleration X: %.2fg Y: %.2fg Z: %.2fg\n", xAccl, yAccl, zAccl);
        printf("Wind Speed: %.2fMPH\n", wind_speed_mph);


        // Write sensor data to memory bank
        FILE *outputFile = fopen(FILENAME, "a");
        if (outputFile == NULL) {
            perror("Failed to open file for writing data to SD Card.");
            return 1;
        }

        fprintf(outputFile, " %s UTC", dateTime);
        fprintf(outputFile, " T_i:%.2fC H_i:%.2f%%", internal_temperature, internal_humidity);
        fprintf(outputFile, " T_e:%.2fC H_e:%.2f%%", external_temperature, external_humidity);
        fprintf(outputFile, " X_o:%.2fg Y_o:%.2fg Z_o:%.2fg", xOffset, yOffset,zOffset);
        fprintf(outputFile, " X:%.2fg Y:%.2fg Z:%.2fg", xAccl, yAccl, zAccl);
        fprintf(outputFile, " W:%.2fMPH\n", wind_speed_mph);
        fclose(outputFile);



        //PIMS-C UART
        SensorReport ttyS1 = {0};
        		strcpy(ttyS1.dateTime, dateTime);
        		ttyS1.xOffset = xOffset;
        		ttyS1.yOffset = yOffset;
        		ttyS1.zOffset = zOffset;
        		ttyS1.xAccl = xAccl;
        		ttyS1.yAccl = yAccl;
        		ttyS1.zAccl = zAccl;
        		ttyS1.external_temperature = external_temperature;
        		ttyS1.external_humidity = external_humidity;
        		ttyS1.internal_temperature = internal_temperature;
        		ttyS1.internal_humidity = internal_humidity;
        		ttyS1.wind_speed_mph = wind_speed_mph;


        transmitSensorReport(ttyS1);


        // Delay for 1 second before next reading
        sleep(1);

    }

    return 1;
}
