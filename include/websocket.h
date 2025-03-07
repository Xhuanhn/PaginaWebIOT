#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

void broadcastSensorValues();
void onWsEvent(AsyncWebSocket *ws, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len);

#endif