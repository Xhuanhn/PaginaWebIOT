#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

void setupUltrasonic();
int readUltrasonic();
void beginTaskUltrasonic();
void formatSensorMessageInt(const char *label, int value, const char *unit, char *buffer, int bufferSize);
#endif