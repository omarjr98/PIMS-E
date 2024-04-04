#include "gpio_control.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define GPIO_PATH_PREFIX "/sys/class/gpio/gpio"

void export_gpio(const char *gpio)
{
    char gpio_path[50];
    sprintf(gpio_path, "%s%s", GPIO_PATH_PREFIX, gpio);
    if (access(gpio_path, F_OK) == -1)
    {
        int export_fd = open("/sys/class/gpio/export", O_WRONLY);
        if (export_fd != -1)
        {
            write(export_fd, gpio, strlen(gpio));
            close(export_fd);
        }
    }
}

char read_value(const char *gpio)
{
    char value = 'E'; // Default to error value
    char path[50];
    sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
    int value_fd = open(path, O_RDONLY);
    if (value_fd != -1)
    {
        if (read(value_fd, &value, 1) == -1)
        {
            value = 'E'; // Set error value if read fails
        }
        close(value_fd);
    }

    return value;
}
