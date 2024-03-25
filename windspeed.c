#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <math.h>

#define I2C_BUS "/dev/i2c-2"
#define ADC_ADDR 0x48
#define CONFIG_REG 0x01
#define CONVERSION_REG 0x00
#define CONFIG_A0 0b0100001000000011
#define CONFIG_A1 0b0101001000000011


int adc_fd;

void init_i2c() {
    if ((adc_fd = open(I2C_BUS, O_RDWR)) < 0) {
        perror("Failed to open interface bus for ADC");
        exit(1);
    }
    if (ioctl(adc_fd, I2C_SLAVE, ADC_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to ADC");
        exit(1);
    }
}

void write_config(uint16_t config_value) {
    uint8_t config_buf[3] = {CONFIG_REG, config_value >> 8, config_value & 0xFF};
    if (write(adc_fd, config_buf, sizeof(config_buf)) != sizeof(config_buf)) {
        perror("Failed to write to ADC configuration register");
        exit(1);
    }
}

void point_to_conversion_register() {
    uint8_t conversion_reg = 0x00;
    if (write(adc_fd, &conversion_reg, sizeof(conversion_reg)) != sizeof(conversion_reg)) {
        perror("Failed to set pointer to conversion register");
        exit(1);
    }
}

int16_t read_conversion() {
    uint8_t conversion_buf[2] = {0};
    if (read(adc_fd, conversion_buf, sizeof(conversion_buf)) != sizeof(conversion_buf)) {
        perror("Failed to read ADC conversion register");
        exit(1);
    }
    return (int16_t)((conversion_buf[0] << 4) | (conversion_buf[1] >> 4));
}

float calculate_wind_speed(float Vout, float Vtemp) {
  // Temperature sensor formula
  float TEMP = (Vtemp - 0.400) / 0.0195;
    // Wind speed formula
    float ws_mph = pow((((Vout - 1.253) / (3.038517 * pow(TEMP, 0.115157))) / 0.087288), 3.009364);
    return ws_mph;
}

int main() {
    init_i2c();

    while (1) {

        // Point and write to Config register
        write_config(CONFIG_A0);

        // Set the pointer to the conversion register
        point_to_conversion_register();

        // Delay before next reading
        usleep(250000);

        // Request the converted byte
        uint16_t converted_value_A0 = read_conversion();

        float Vout = converted_value_A0 * 0.002; // 2mV LSB
        printf("VOUT: %.3f V\n", Vout);

        // Point and write to Config register
        write_config(CONFIG_A1);

        // Set the pointer to the conversion register
        point_to_conversion_register();

        // Delay before next reading
        usleep(250000);

        // Request the converted byte
        uint16_t converted_value_A1 = read_conversion();

        float Vtemp = converted_value_A1 * 0.002; // 2mV LSB
        printf("VTEMP: %.3f V\n", Vtemp);

        // Print temperature
        float TEMP = (Vtemp - 0.400) / 0.0195;
        printf("TEMP: %.2f C\n", TEMP);

        // Calculate wind speed
        float wind_speed_mph = calculate_wind_speed(Vout, Vtemp);

        // Print wind speed
        printf("Wind Speed (mph): %.2f\n", wind_speed_mph);

        // Delay before next reading
        usleep(200000);

    }

    close(adc_fd);
    return 0;
}
