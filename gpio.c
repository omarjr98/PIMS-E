#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_PATH_PREFIX "/sys/class/gpio/gpio"

void export_gpio(const char *gpio)
{
    char path[50];
    sprintf(path, "%s%s", GPIO_PATH_PREFIX, gpio);
    if (access(path, F_OK) != -1) {
        //printf("GPIO pin %s already exported.\n", gpio);
        return;
    }

    int export_fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (export_fd == -1)
    {
        perror("Error exporting GPIO pin");
        return;
    }

    if (write(export_fd, gpio, strlen(gpio)) == -1)
    {
        perror("Error writing GPIO export");
    }

    close(export_fd);
}

void set_direction(const char *gpio, const char *direction)
{
    char path[50];
    int direction_fd;
    sprintf(path, "%s%s/direction", GPIO_PATH_PREFIX, gpio);
    direction_fd = open(path, O_WRONLY);
    if (direction_fd == -1) {
        perror("Error opening GPIO direction file");
        return;
    }

    if (write(direction_fd, direction, strlen(direction)) == -1) {
        perror("Error writing GPIO direction");
    }

    close(direction_fd);
}

char read_value(const char *gpio)
{
    char value;
    char path[50];
    int value_fd;
    sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
    value_fd = open(path, O_RDONLY);
    if (value_fd == -1) {
        perror("Error opening GPIO value file");
        return '0'; // Return a default value or handle error as required
    }

    if (read(value_fd, &value, 1) == -1) {
        perror("Error reading GPIO value");
        value = '0'; // Return a default value or handle error as required
    }

    close(value_fd);
    return value;
}

void set_value(const char *gpio, const char *value)
{
    char path[50];
    int value_fd;
    sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
    value_fd = open(path, O_WRONLY);
    if (value_fd == -1) {
        perror("Error opening GPIO value file");
        return;
    }

    if (write(value_fd, value, strlen(value)) == -1) {
        perror("Error writing GPIO value");
    }

    close(value_fd);
}
