#include <Arduino.h>
#include <esp_system.h>
#include "config.h"
#include "websocket.h"
#include "identification-module.h"
#include "flow-sensor.h"
#include "pressure-sensor.h"
#include "ultrasonic-sensor.h"
#include "mqtt-client.h"
#include <SPIFFS.h>
#include "websocket.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("🚀 Iniciando ESP32...");

    // Inicializar preferencias
    preferences.begin("config", false);
    Serial.println("✅ Preferencias inicializadas.");

    // Montar SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("❌ Error al montar SPIFFS");
        return;
    }
    Serial.println("✅ SPIFFS montado correctamente.");

    // Configuración del servidor
    Serial.println("DEBUG: Configurando servidor...");
    server.on("/ip.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/ip.txt", "text/plain");
    });

    server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", WiFi.localIP().toString());
    });

    // Conectar WiFi
    Serial.println("DEBUG: Configurando WiFi...");
    setupWiFi();
    Serial.println("✅ WiFi configurado.");
    // Inicializar sensores
    Serial.println("DEBUG: Configurando Sensores...");
    setupUltrasonic();
    setupPressure();
    setupFlow();
 
    beginTaskFlow();
    beginTaskPressure();
    beginTaskUltrasonic();
    Serial.println("✅ Sensores configurados.");

    delay(1000);

    // Configurar WebSocket
    Serial.println("DEBUG: Configurando WebSocket...");
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // Servir archivos
    server.serveStatic("/css", SPIFFS, "/css");
    server.serveStatic("/js", SPIFFS, "/js");
    server.serveStatic("/source", SPIFFS, "/source");

    // Configuración de rutas
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/login.html", "text/html");
    });

    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // Iniciar servidor web
    Serial.println("DEBUG: Iniciando servidor web...");
    server.begin();
    Serial.println("✅ Servidor web iniciado.");
    // Iniciar MQTT
    Serial.println("DEBUG: Iniciando MQTT...");
    beginMQTT();
    Serial.println("✅ MQTT configurado.");
    Serial.println("🎯 Setup finalizado. Iniciando loop...");
}


void loop()
{
    Serial.println("🔄 Ejecutando loop...");
    client.loop(); 
    ws.cleanupClients();  
    broadcastSensorValues(); 
    delay(2000);
}
