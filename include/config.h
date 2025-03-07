#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

extern String mqtt_broker;
extern String mqtt_user;
extern String mqtt_password;
extern String mqtt_root_topic;
extern bool mqtt_enabled;

extern Preferences preferences;
extern AsyncWebServer server;
extern AsyncWebSocket ws;

void checkWiFiStatus(AsyncWebSocketClient *client);
void sendFullConfig(AsyncWebSocketClient *client);
void setupWiFi();

#endif
