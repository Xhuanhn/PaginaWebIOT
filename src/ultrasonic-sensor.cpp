#include "../include/ultrasonic-sensor.h"
#include "../include/mqtt-client.h"
#include <Arduino.h>
#include "../include/config.h"

#define TRIG_PIN 5
#define ECHO_PIN 18

void setupUltrasonic() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

int readUltrasonic() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracion = pulseIn(ECHO_PIN, HIGH);
    return duracion * 0.034 / 2;  
}

void formatSensorMessageInt(const char *label, int value, const char *unit, char *buffer, int bufferSize) {
    snprintf(buffer, bufferSize, "%s: %d %s\n", label, value, unit);
}


void readUltrasonicTask(void *parameter) {
    String root_topic = preferences.getString("mqttRootTopic", "No configurado") + "sensor/ultrasonic";
    char mqtt_topic[100];
    root_topic.toCharArray(mqtt_topic, sizeof(mqtt_topic));

    while (1) {
        int distancia = readUltrasonic();
        char message[50];
        formatSensorMessageInt("Distancia", distancia, "cm", message, sizeof(message));

        vTaskDelay((1500 + (esp_random() % 500)) / portTICK_PERIOD_MS); 
        publishMQTT(mqtt_topic, message);
    }
}



void beginTaskUltrasonic() {
    xTaskCreatePinnedToCore(readUltrasonicTask, "LeerUltrasonico", 4096, NULL, 1, NULL, 1);
}