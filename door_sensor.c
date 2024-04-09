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

// Register addresses
#define TMAG5273_I2C_ADDRESS 0x22
#define TMAG5273_REG_DEVICE_CONFIG_1 0x00
#define TMAG5273_REG_DEVICE_CONFIG_2 0x01
#define TMAG5273_REG_SENSOR_CONFIG_1 0x02
#define TMAG5273_REG_SENSOR_CONFIG_2 0x03
#define TMAG5273_REG_GAIN_VALUE 0x09
#define TMAG5273_REG_Z_MSB_RESULT 0x16
#define TMAG5273_REG_Z_LSB_RESULT 0x17

// Register values
#define DEVICE_CONFIG_1_VALUE 0x30
#define DEVICE_CONFIG_2_VALUE 0x02
#define SENSOR_CONFIG_1_VALUE 0x40
#define SENSOR_CONFIG_2_VALUE 0x01

#define I2C_BUS "/dev/i2c-2"
#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_PATH_PREFIX "/sys/class/gpio/gpio"
#define LED_PIN "66"

int door_sensor;

// Function to open the I2C bus
void init_i2c() {
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

// Function to close the I2C bus
void closeI2C() {
    if (door_sensor >= 0) {
        close(door_sensor);
        door_sensor = -1;
    }
}

// Function to write a byte to registers
int writeRegister(unsigned char reg, unsigned char value) {
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

// Function to read a byte from registers
int readRegister(unsigned char reg, unsigned char *value) {
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

void export_gpio(const char *gpio)
{
    char path[50];
    sprintf(path, "%s%s", GPIO_PATH_PREFIX, gpio);
    if (access(path, F_OK) != -1) {
        printf("GPIO pin %s already exported.\n", gpio);
        return;
    }

    int export_fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (export_fd == -1)
    {
        perror("Error exporting maintenance light GPIO");
        return;
    }

    if (write(export_fd, gpio, strlen(gpio)) == -1)
    {
        perror("Error writing maintenance light GPIO export");
    }

    close(export_fd);
}

void set_direction(const char *gpio, const char *direction)
{
    char path[50];
    int direction_fd;
    int retries = 3;
    while (retries > 0) {
        sprintf(path, "%s%s/direction", GPIO_PATH_PREFIX, gpio);
        direction_fd = open(path, O_WRONLY);
        if (direction_fd == -1) {
            perror("Error opening maintenance light GPIO direction file");
            retries--;
            sleep(1);
        } else {
            break; // Success
        }
    }
    if (retries == 0) {
        fprintf(stderr, "Failed to open maintenance light GPIO direction file after multiple retries\n");
        exit(1);
    }

    retries = 3;
    while (retries > 0) {
        if (write(direction_fd, direction, strlen(direction)) == -1) {
            perror("Error writing maintenance light GPIO direction");
            retries--;
            sleep(1);
        } else {
            break; // Success
        }
    }
    if (retries == 0) {
        fprintf(stderr, "Failed to write maintenance light GPIO direction after multiple retries\n");
        close(direction_fd);
        exit(1);
    }

    close(direction_fd);
}

char read_value(const char *gpio)
{
    char value;
    char path[50];
    int value_fd;
    int retries = 3;
    while (retries > 0) {
        sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
        value_fd = open(path, O_RDONLY);
        if (value_fd == -1) {
            perror("Error opening maintenance light GPIO value file");
            retries--;
            sleep(1);
        } else {
            break; // Success
        }
    }
    if (retries == 0) {
        fprintf(stderr, "Failed to open maintenance light GPIO value file after multiple retries\n");
        exit(1);
    }

    retries = 3;
    while (retries > 0) {
        if (read(value_fd, &value, 1) == -1) {
            perror("Error reading maintenance light GPIO value");
            retries--;
            sleep(1);
        } else {
            break; // Success
        }
    }
    if (retries == 0) {
        fprintf(stderr, "Failed to read maintenance light GPIO value after multiple retries\n");
        close(value_fd);
        exit(1);
    }

    close(value_fd);
    return value;
}

void set_value(const char *gpio, const char *value)
{
    char path[50];
    int value_fd;
    int retries = 3;
    while (retries > 0) {
        sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
        value_fd = open(path, O_WRONLY);
        if (value_fd == -1) {
            perror("Error opening maintenance light GPIO value file");
            retries--;
            sleep(1);
        } else {
            break; // Success
        }
    }
    if (retries == 0) {
        fprintf(stderr, "Failed to open maintenance light GPIO value file after multiple retries\n");
        exit(1);
    }

    retries = 3;
    while (retries > 0) {
        if (write(value_fd, value, strlen(value)) == -1) {
            perror("Error writing maintenance light GPIO value");
            retries--;
            sleep(1);
        } else {
            break; // Success
        }
    }
    if (retries == 0) {
        fprintf(stderr, "Failed to write maintenance light GPIO value after multiple retries\n");
        close(value_fd);
        exit(1);
    }

    close(value_fd);
}

int main() {
    // Open I2C bus
    init_i2c();

    // Gain value
    unsigned char gain_value = 128; // Adjust as needed

    // Set register values
    if (writeRegister(TMAG5273_REG_DEVICE_CONFIG_1, DEVICE_CONFIG_1_VALUE) == -1 ||
        writeRegister(TMAG5273_REG_DEVICE_CONFIG_2, DEVICE_CONFIG_2_VALUE) == -1 ||
        writeRegister(TMAG5273_REG_SENSOR_CONFIG_1, SENSOR_CONFIG_1_VALUE) == -1 ||
        writeRegister(TMAG5273_REG_SENSOR_CONFIG_2, SENSOR_CONFIG_2_VALUE) == -1 ||
        writeRegister(TMAG5273_REG_GAIN_VALUE, gain_value) == -1)
    {
        fprintf(stderr, "Failed to set register values\n");
        closeI2C();
        exit(1);
    }

    // Export GPIO pins
    export_gpio(LED_PIN);

    // Set GPIO pin directions
    set_direction(LED_PIN, "out");

    // Variables for reading sensor value
    char sensor_value;

    while (1)
    {
        // Read sensor value
        unsigned char z_msb_result, z_lsb_result;
        if (readRegister(TMAG5273_REG_Z_MSB_RESULT, &z_msb_result) == -1 ||
            readRegister(TMAG5273_REG_Z_LSB_RESULT, &z_lsb_result) == -1)
        {
            fprintf(stderr, "Failed to read sensor values\n");
            closeI2C();
            exit(1);
        }

        // Combine MSB and LSB to get the Z-axis value as a signed 16-bit integer
        int16_t z_value = (z_msb_result << 8) | z_lsb_result;

        // Make raw value absolute
        int16_t abs_z_value = abs(z_value);

        // Print the absolute Z-axis value
        printf("Z-axis value: %" PRId16 "\n", abs_z_value);

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

        sleep(1);
    }

    closeI2C();

    return 0;
}
