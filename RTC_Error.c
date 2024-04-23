#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

// GPIO Pin Definitions
#define GPIO_LED_PIN 66

#define RTC_DEVICE "/dev/rtc1"

// Function to turn on LED connected to GPIO pin 66
void turn_on_led() {
    int gpio_fd = open("/sys/class/gpio/gpio66/value", O_WRONLY);
    if (gpio_fd == -1) {
        perror("Error opening GPIO pin for LED");
        exit(EXIT_FAILURE);
    }
    // Write "1" to turn on the LED
    if (write(gpio_fd, "1", 1) != 1) {
        perror("Error writing to GPIO pin");
        exit(EXIT_FAILURE);
    }
    close(gpio_fd);
}

// Function to turn off LED connected to GPIO pin 66
void turn_off_led() {
    int gpio_fd = open("/sys/class/gpio/gpio66/value", O_WRONLY);
    if (gpio_fd == -1) {
        perror("Error opening GPIO pin for LED");
        exit(EXIT_FAILURE);
    }
    // Write "0" to turn off the LED
    if (write(gpio_fd, "0", 1) != 1) {
        perror("Error writing to GPIO pin");
        exit(EXIT_FAILURE);
    }
    close(gpio_fd);
}

// Function to toggle LED connected to GPIO pin 66
void toggle_led() {
    int gpio_fd = open("/sys/class/gpio/gpio66/value", O_RDWR);
    if (gpio_fd == -1) {
        perror("Error opening GPIO pin for LED");
        exit(EXIT_FAILURE);
    }
    char value;
    if (read(gpio_fd, &value, 1) != 1) {
        perror("Error reading from GPIO pin");
        exit(EXIT_FAILURE);
    }
    // Toggle LED state
    value = (value == '0') ? '1' : '0';
    if (lseek(gpio_fd, 0, SEEK_SET) == -1) {
        perror("Error seeking GPIO pin");
        exit(EXIT_FAILURE);
    }
    if (write(gpio_fd, &value, 1) != 1) {
        perror("Error writing to GPIO pin");
        exit(EXIT_FAILURE);
    }
    close(gpio_fd);
}

int main() {
    int rtc_fd;
    struct rtc_time rtc_tm;
    int rtc_failed = 0;

    while (1) {
        // Open the RTC device
        rtc_fd = open(RTC_DEVICE, O_RDONLY);
        if (rtc_fd == -1) {
            if (!rtc_failed) {
                perror("Failed to open RTC device");
                turn_on_led(); // Turn on LED if failed to open RTC device
                rtc_failed = 1;
            } else {
                // Toggle LED every second if RTC device still can't be opened
                toggle_led();
            }
        } else {
            // If RTC device is opened successfully, turn off LED and reset rtc_failed flag
            if (rtc_failed) {
                turn_off_led();
                rtc_failed = 0;
            }
            // Read the current time from the RTC
            if (ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm) == -1) {
                perror("Failed to read RTC time");
                close(rtc_fd);
                turn_on_led(); // Turn on LED if failed to read RTC time
                rtc_failed = 1;
            } else {
                // Print current date and time
                printf("Current date and time: %04d-%02d-%02d %02d:%02d:%02d\n",
                       rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday,
                       rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
                // Close RTC device
                close(rtc_fd);
            }
        }
        // Delay for 1 second before next reading
        sleep(1);
    }

    return 0;
}
