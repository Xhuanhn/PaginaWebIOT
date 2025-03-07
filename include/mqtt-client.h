#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>  // Asegúrate de incluir esto

extern PubSubClient client; // <--- Añade esta línea para usar el cliente en otros archivos

void conectMQTT();
void beginMQTT();
void publishMQTT(const char *topic, const char *mensaje);

#endif
