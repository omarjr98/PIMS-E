
//=================== Sensor Report =========================//

typedef struct SensorReport{

    char dateTime[DATETIME_LENGTH];
    float xOffset, yOffset, zOffset;
    float xAccl, yAccl, zAccl;
    float external_temperature, external_humidity;
    float internal_temperature, internal_humidity;
    float wind_speed_mph;
    // float ultrasound;

} SensorReport;

#define SENSOR_REPORT_SIZE (unsigned char)sizeof(SensorReport)

#endif 
