#ifndef SENSOR_REPORT_H
#define SENSOR_REPORT_H
#include "internal_temp_humidity.h"
#include "external_temp_humidity.h"
#include "accelerometer.h"
#include "calibration.h"
#include "wind_sensor.h"
#include "time.h"

//=================== Sensor Report =========================//

typedef struct SensorReport{

    char dateTime[20];
    float xOffset;
	float yOffset;
	float zOffset;
    float xAccl;
	float yAccl;
	float zAccl;
    float external_temperature;
    float external_humidity;
    float internal_temperature;
	float internal_humidity;
    float wind_speed_mph;
  //float ultrasound;

} SensorReport;

#define SENSOR_REPORT_SIZE (unsigned char)sizeof(SensorReport)

#endif
