#include <Arduino.h>
#include <esp_system.h>
#include <esp_task_wdt.h>  // 📌 Librería para el Watchdog
#include "config.h"  // Contiene setupWiFi()
#include "websocket.h"
#include "identification-module.h"
#include "flow-sensor.h"
#include "pressure-sensor.h"
#include "ultrasonic-sensor.h"
#include "mqtt-client.h"
#include <SPIFFS.h>
#include "websocket.h"

#define RESET_BUTTON_PIN 14  // GPIO del botón de reset
#define RESET_HOLD_TIME 10000  // 10 segundos

void setup()
{
    Serial.begin(115200);
    Serial.println("🚀 Iniciando ESP32...");

    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

    // 📌 Inicializar Watchdog Timer (WDT)
    esp_task_wdt_init(5, true);  // Tiempo máximo de bloqueo: 5s
    esp_task_wdt_add(NULL);  // Monitorear la tarea principal (loopTask)

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
    
    // 📌 Resetear el Watchdog Timer para evitar reinicio inesperado
    esp_task_wdt_reset();

    // ✅ Detectar si el botón de reset ha sido presionado por 10 segundos
    unsigned long buttonPressStart = 0;
    bool buttonHeld = false;

    if (digitalRead(RESET_BUTTON_PIN) == LOW) {
        buttonPressStart = millis();
        Serial.println("🟠 Botón de reset presionado, esperando 10s...");
        
        while (digitalRead(RESET_BUTTON_PIN) == LOW) {  
            if (millis() - buttonPressStart >= RESET_HOLD_TIME) {
                Serial.println("🛑 Botón de reset presionado por 10s! Restableciendo WiFi...");
                buttonHeld = true;
                break;
            }
            esp_task_wdt_reset();
            delay(100);  // Pequeña pausa para evitar lectura errónea
        }

        if (buttonHeld) {
            Serial.println("⚠️ Borrando preferencias de WiFi...");
            preferences.begin("config", false);
            preferences.clear();
            preferences.end();
            Serial.println("✅ Preferencias eliminadas.");

            Serial.println("♻️ Reiniciando WiFi...");
            setupWiFi(); 
        } else {
            Serial.println("🔵 Botón soltado antes de 10s, cancelando reset.");
        }
    }

    client.loop();  // Mantiene conexión MQTT
    ws.cleanupClients();  // Mantiene WebSocket activo
    broadcastSensorValues();  // Envía datos de sensores

    esp_task_wdt_reset();
    
    delay(2000);
}
