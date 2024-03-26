#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define SENSOR_PIN "60"
#define LED_PIN "66"
#define GPIO_PATH_PREFIX "/sys/class/gpio/gpio"

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

int main()
{
    // Export GPIO pins
    export_gpio(SENSOR_PIN);
    export_gpio(LED_PIN);

    // Set GPIO pin directions
    set_direction(SENSOR_PIN, "in");
    set_direction(LED_PIN, "out");

    // Variables for reading sensor value
    char sensor_value;

    while (1)
    {
        // Read sensor value
        sensor_value = read_value(SENSOR_PIN);

        // Debug print statement
        printf("Sensor Value: %c\n", sensor_value);

        // Control the LED based on sensor value
        if (sensor_value == '0') // Magnet detected
            set_value(LED_PIN, "0"); // Turn off the LED
        else
            set_value(LED_PIN, "1"); // Turn on the LED

        usleep(100000); // Delay for stability (100ms)
    }

    return 0;
}
