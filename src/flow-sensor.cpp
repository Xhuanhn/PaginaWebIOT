#include "../include/flow-sensor.h"
#include "../include/mqtt-client.h"
#include <Arduino.h>
#include "../include/config.h"

const int flowPin = 2;
volatile int pulseCount = 0;
const float calibrationFactor = 4.5;

void countPulse() {
    pulseCount++;
}

void setupFlow() {
    pinMode(flowPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(flowPin), countPulse, FALLING);
}

float readFlow() {
    float flowRate = pulseCount / calibrationFactor;
    pulseCount = 0;
    return flowRate;
}


void formatFlowMessage(float flujo, char *buffer, int bufferSize) {
    snprintf(buffer, bufferSize, "Flujo: %.2f L/min\n", flujo);
}

void readFlowTask(void *parameter) {
    String root_topic = preferences.getString("mqttRootTopic", "No configurado") + "sensor/flow";
    char mqtt_topic[100];
    root_topic.toCharArray(mqtt_topic, sizeof(mqtt_topic));

    while (1) {
        float flujo = readFlow();
        char message[50];
        formatFlowMessage(flujo, message, sizeof(message));

        vTaskDelay((1500 + (esp_random() % 500)) / portTICK_PERIOD_MS); 
        publishMQTT(mqtt_topic, message);
    }
}



void beginTaskFlow() {
    xTaskCreatePinnedToCore(readFlowTask, "LeerFlujo", 4096, NULL, 1, NULL, 1);
}