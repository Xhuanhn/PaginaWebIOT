#include "../include/pressure-sensor.h"
#include "../include/mqtt-client.h"
#include <Arduino.h>
#include "../include/config.h"

const int pressureInput = 32;
const float pressureZero = 432.0;
const float pressureMax = 921.6;
const float pressureTransducerMaxPsi = 100;

void setupPressure() {
    pinMode(pressureInput, INPUT);
}

float readPressure() {
    int pressure = analogRead(pressureInput);
    return (pressure - pressureZero) * pressureTransducerMaxPsi / (pressureMax - pressureZero);
}

void formatSensorMessage(const char *label, float value, const char *unit, char *buffer, int bufferSize) {
    snprintf(buffer, bufferSize, "%s: %.2f %s\n", label, value, unit);
}

void readPressureTask(void *parameter) {
    String root_topic = preferences.getString("mqttRootTopic", "No configurado") + "sensor/pressure";
    char mqtt_topic[100];
    root_topic.toCharArray(mqtt_topic, sizeof(mqtt_topic));

    while (1) {
        float presion = readPressure();
        char message[50];
        formatSensorMessage("Presión", presion, "bar", message, sizeof(message));

        vTaskDelay((1500 + (esp_random() % 500)) / portTICK_PERIOD_MS); // Delay aleatorio
        publishMQTT(mqtt_topic, message);
    }
}



void beginTaskPressure() {
    xTaskCreatePinnedToCore(readPressureTask, "LeerPresion", 4096, NULL, 1, NULL, 1);
}