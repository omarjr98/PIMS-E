#include "door_sensor.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#define I2C_BUS "/dev/i2c-2"

#define TMAG5273_I2C_ADDRESS 0x22
#define TMAG5273_REG_DEVICE_CONFIG_1 0x00
#define TMAG5273_REG_DEVICE_CONFIG_2 0x01
#define TMAG5273_REG_SENSOR_CONFIG_1 0x02
#define TMAG5273_REG_SENSOR_CONFIG_2 0x03
#define TMAG5273_REG_GAIN_VALUE 0x09
#define TMAG5273_REG_Z_MSB_RESULT 0x16
#define TMAG5273_REG_Z_LSB_RESULT 0x17

#define DEVICE_CONFIG_1_VALUE 0x30
#define DEVICE_CONFIG_2_VALUE 0x02
#define SENSOR_CONFIG_1_VALUE 0x40
#define SENSOR_CONFIG_2_VALUE 0x01

#define LED_PIN "66"

static int door_sensor;

static void init_i2c() {
    // Initialization code for I2C bus
	int retries = 3;
	    while (retries > 0) {
	        if ((door_sensor = open(I2C_BUS, O_RDWR)) < 0) {
	            perror("Failed to open I2C BUS for door sensor");
	            retries--;
	            sleep(1);
	        } else {
	            break; // Success
	        }
	    }
	    if (retries == 0) {
	        fprintf(stderr, "Failed to open I2C BUS after multiple retries\n");
	        exit(1);
	    }

	    retries = 3;
	    while (retries > 0) {
	        if (ioctl(door_sensor, I2C_SLAVE, TMAG5273_I2C_ADDRESS) < 0) {
	            perror("Failed to acquire I2C bus access and/or talk to door sensor");
	            retries--;
	            sleep(1);
	        } else {
	            break; // Success
	        }
	    }
	    if (retries == 0) {
	        fprintf(stderr, "Failed to acquire I2C bus access after multiple retries\n");
	        close(door_sensor);
	        exit(1);
	    }
}

static void close_i2c() {
    // Cleanup code for closing I2C bus
	if (door_sensor >= 0) {
	        close(door_sensor);
	        door_sensor = -1;
	    }
}

static int write_register(unsigned char reg, unsigned char value) {
    // Code for writing to I2C register
	 int retries = 3;
	    while (retries > 0) {
	        if (door_sensor < 0) {
	            fprintf(stderr, "I2C BUS not initialized for door sensor\n");
	            return -1;
	        }
	        unsigned char buffer[2] = { reg, value };
	        if (write(door_sensor, buffer, 2) != 2) {
	            perror("Failed to write to door sensor configuration register");
	            retries--;
	            sleep(1);
	        } else {
	            return 1; // Success
	        }
	    }
	    fprintf(stderr, "Failed to write to door sensor configuration register after multiple retries\n");
	    return -1;
}

static int read_register(unsigned char reg, unsigned char *value) {
    // Code for reading from I2C register
	 int retries = 3;
	    while (retries > 0) {
	        if (door_sensor < 0) {
	            fprintf(stderr, "Door sensor I2C BUS not initialized\n");
	            return -1;
	        }
	        if (write(door_sensor, &reg, 1) != 1) {
	            perror("Failed to write to door sensor configuration register");
	            retries--;
	            sleep(1);
	        } else {
	            if (read(door_sensor, value, 1) != 1) {
	                perror("Failed to read door sensor registers");
	                retries--;
	                sleep(1);
	            } else {
	                return 1; // Success
	            }
	        }
	    }
	    fprintf(stderr, "Failed to read door sensor registers after multiple retries\n");
	    return -1;
}

void door_sensor_main() {
    // Initialization
    init_i2c();

    // Gain value
    unsigned char gain_value = 128;

    // Set register values
    if (write_register(TMAG5273_REG_DEVICE_CONFIG_1, DEVICE_CONFIG_1_VALUE) == -1 ||
        write_register(TMAG5273_REG_DEVICE_CONFIG_2, DEVICE_CONFIG_2_VALUE) == -1 ||
        write_register(TMAG5273_REG_SENSOR_CONFIG_1, SENSOR_CONFIG_1_VALUE) == -1 ||
        write_register(TMAG5273_REG_SENSOR_CONFIG_2, SENSOR_CONFIG_2_VALUE) == -1 ||
        write_register(TMAG5273_REG_GAIN_VALUE, gain_value) == -1)
    {
        fprintf(stderr, "Failed to set register values\n");
        close_i2c();
        exit(1);
    }

    // Export GPIO pins
    export_gpio(LED_PIN);

    // Set GPIO pin directions
    set_direction(LED_PIN, "out");

    // Variables for reading sensor value
    char sensor_value;

        // Read sensor value
        unsigned char z_msb_result, z_lsb_result;
        if (read_register(TMAG5273_REG_Z_MSB_RESULT, &z_msb_result) == -1 ||
            read_register(TMAG5273_REG_Z_LSB_RESULT, &z_lsb_result) == -1)
        {
            fprintf(stderr, "Failed to read sensor values\n");
            close_i2c();
            exit(1);
        }

        // Combine MSB and LSB to get the Z-axis value as a signed 16-bit integer
        int16_t z_value = (z_msb_result << 8) | z_lsb_result;

        // Make raw value absolute
        int16_t abs_z_value = abs(z_value);

        // Door status logic
        if (abs_z_value <= 260) // Magnet detected
        {
            printf("Door open\n");
            set_value(LED_PIN, "1"); // Turn on LED
        }
        else
        {
            printf("Door close\n");
            set_value(LED_PIN, "0"); // Turn off LED
        }
        close_i2c();
    }
