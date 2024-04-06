#ifndef wind_sensor_h
#define wind_sensor_h

// Function declaration for reading wind speed
void read_wind_speed(float *Vtemp, float *wind_speed_mph, float *TEMP);
void init_i2c();

#endif
