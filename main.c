#include "external_temp_humidity_h.h"
#include "internal_temp_humidity_h.h"
#include "acceleration.h"
#include "calibration.h"
#include <unistd.h>
#include <stdio.h>

void print_binary(FILE *file, unsigned int value, int num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
        fprintf(file, "%d", (value >> i) & 1);
    }
}

int main() {

	//calibrate_offsets(float *x_offset, float *y_offset, float *z_offset);// Add the void keyword to the function declaration

    while (1) {
        // Read data from the internal and external sensor
    	init_accelerometer();
        external_temp_humidity();
        internal_temp_humidity();
        sleep(20);
        // Calibrate the accelerometer
        float x_offset, y_offset, z_offset;
       calibrate_offsets(&x_offset, &y_offset, &z_offset); // Add the necessary arguments to the function call

        // Read acceleration data
        float x_accel = read_acceleration_x();
        float y_accel = read_acceleration_y();
        float z_accel = read_acceleration_z();

        // Do something with the acceleration data...


    }
    return 0;
}

    return 0;
}
