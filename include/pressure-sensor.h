#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

void setupPressure();
float readPressure();
void beginTaskPressure();
void formatSensorMessage(const char *label, float value, const char *unit, char *buffer, int bufferSize);

#endif
