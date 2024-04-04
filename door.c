#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_PATH_PREFIX "/sys/class/gpio/gpio"

void export_gpio(const char *gpio);
void set_direction(const char *gpio, const char *direction);
char read_value(const char *gpio);
void set_value(const char *gpio, const char *value);

void setup_gpio()
{
    // Export GPIO pins
    export_gpio(SENSOR_PIN);
    export_gpio(LED_PIN);

    // Set GPIO pin directions
    set_direction(SENSOR_PIN, "in");
    set_direction(LED_PIN, "out");
}

void export_gpio(const char *gpio)
{
    int export_fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (export_fd == -1)
    {
        perror("Error exporting GPIO");
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
    sprintf(path, "%s%s/direction", GPIO_PATH_PREFIX, gpio);
    int direction_fd = open(path, O_WRONLY);
    if (direction_fd == -1)
    {
        perror("Error opening direction file");
        return;
    }

    if (write(direction_fd, direction, strlen(direction)) == -1)
    {
        perror("Error writing direction");
    }

    close(direction_fd);
}

char read_value(const char *gpio)
{
    char value;
    char path[50];
    sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
    int value_fd = open(path, O_RDONLY);
    if (value_fd == -1)
    {
        perror("Error opening value file");
        return 'E'; // Return 'E' for error
    }

    if (read(value_fd, &value, 1) == -1)
    {
        perror("Error reading value");
        close(value_fd);
        return 'E'; // Return 'E' for error
    }

    close(value_fd);
    return value;
}

void set_value(const char *gpio, const char *value)
{
    char path[50];
    sprintf(path, "%s%s/value", GPIO_PATH_PREFIX, gpio);
    int value_fd = open(path, O_WRONLY);
    if (value_fd == -1)
    {
        perror("Error opening value file");
        return;
    }

    if (write(value_fd, value, strlen(value)) == -1)
    {
        perror("Error writing value");
    }

    close(value_fd);
}
