#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

void setupFlow();
float readFlow();
void beginTaskFlow();
void formatFlowMessage(float flujo, char *buffer, int bufferSize);
#endif
