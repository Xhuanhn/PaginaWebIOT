#include "../include/mqtt-client.h"
#include "../include/config.h"
#include <PubSubClient.h>
#include <WiFi.h>

// Cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

SemaphoreHandle_t mqttMutex  = xSemaphoreCreateMutex();

void conectMQTT()
{

    bool mqtt_enabled = preferences.getBool("mqttEnabled", false);
    String mqtt_broker = preferences.getString("mqttBroker", "No configurado");
    String mqtt_user = preferences.getString("mqttUser", "No configurado");
    String mqtt_password = preferences.getString("mqttPassword", "No configurado");

    if (!mqtt_enabled)
    {
        Serial.println("MQTT deshabilitado en la configuración.");
        return;
    }
    
    /*
    Serial.print("Esperando clientes en Wi-Fi AP...");
    while (WiFi.softAPgetStationNum() == 0)
    { // Espera hasta que un dispositivo se conecte
        Serial.print(".");
        delay(1000);
    }
    */

    

    Serial.print("Conectando a MQTT...");
    client.setServer(mqtt_broker.c_str(), 1883);
    delay(2000);

    while (!client.connected())
    {
        Serial.print(".");
        if (client.connect("ESP32_Client"))
        {
            Serial.println("¡Conectado!");
        }
        else
        {
            Serial.print("Error rc=");
            Serial.print(client.state());
            Serial.println("Intentando de nuevo...");
            delay(2000);
        }
    }
}

void beginMQTT()
{
    Serial.println("🔌 Conectando a MQTT...");

    bool mqtt_enabled = preferences.getBool("mqttEnabled", false);
    String mqtt_broker = preferences.getString("mqttBroker", "No configurado");
    String mqtt_user = preferences.getString("mqttUser", "No configurado");
    String mqtt_password = preferences.getString("mqttPassword", "No configurado");

    Serial.print("MQTT Habilitado: ");
    Serial.println(mqtt_enabled ? "Sí" : "No");
    Serial.print("Broker MQTT: ");
    Serial.println(mqtt_broker);
    Serial.print("Usuario MQTT: ");
    Serial.println(mqtt_user);
    Serial.print("Contraseña MQTT: ");
    Serial.println(mqtt_password);

    client.setServer(mqtt_broker.c_str(), 1883);

    if (!client.connected())
    {
        Serial.println("Intentando conectar MQTT...");
        if (client.connect("ESP32_Client"))
        {
            Serial.println("✅ MQTT Conectado exitosamente.");
        }
        else
        {
            Serial.print("❌ Error de conexión MQTT, estado=");
            Serial.println(client.state());
        }
    }

    if (!mqtt_enabled)
    {
        Serial.println("⚠️ MQTT está deshabilitado en la configuración.");
        return;
    }

    Serial.print("Intentando conectar a MQTT en: ");
    Serial.println(mqtt_broker);

    client.setServer(mqtt_broker.c_str(), 1883);

    for (int i = 0; i < 5; i++) // Reintentar máximo 5 veces
    {
        if (client.connect("ESP32_Client"))
        {
            Serial.println("✅ Conectado a MQTT!");
            return;
        }
        Serial.print("❌ Error rc=");
        Serial.print(client.state());
        Serial.println(" - Intentando de nuevo...");
        delay(2000);
    }

    Serial.println("❌ No se pudo conectar a MQTT después de 5 intentos.");
}

void publishMQTT(const char *topic, const char *mensaje)
{
    if (xSemaphoreTake(mqttMutex, portMAX_DELAY) == pdTRUE)
    {
        if (!client.connected())
        {
            conectMQTT();
        }
        Serial.println("here");
        client.publish(topic, mensaje);
        xSemaphoreGive(mqttMutex); // Libera el mutex
    }
}
