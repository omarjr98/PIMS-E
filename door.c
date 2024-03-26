#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SENSOR_PIN "60"
#define LED_PIN "66"
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

//int main()
{
    export_gpio(SENSOR_PIN);
    export_gpio(LED_PIN);

    while (1)
    {
       char sensor_value = read_value(SENSOR_PIN);

       if (sensor_value != '1')
       {
           int led_fd = open("/sys/class/gpio/gpio66/value", O_WRONLY);
           if (led_fd != -1)
           {
               write(led_fd, "1", 1); // Turn on LED
               close(led_fd);
           }
       }

       usleep(1000); // Check every millisecond
   }

   return 0;
}
