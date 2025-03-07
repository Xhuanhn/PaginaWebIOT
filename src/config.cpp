#include "../include/config.h"

String mqtt_broker;
String mqtt_user;
String mqtt_password;
String mqtt_root_topic;
bool mqtt_enabled;

AsyncWebServer server(80);   
AsyncWebSocket ws("/ws");

Preferences preferences;

void checkWiFiStatus(AsyncWebSocketClient *client)
{
    JsonDocument response;
    response["status"] = "wifi_status";
    if (WiFi.status() == WL_CONNECTED)
    {
        response["wifiStatus"] = "Conectado";
        response["ip"] = WiFi.localIP().toString();
    }
    else
    {
        response["wifiStatus"] = "Desconectado";
    }
    String responseStr;
    serializeJson(response, responseStr);
    client->text(responseStr); 
}

void sendFullConfig(AsyncWebSocketClient *client)
{
    String ssid = preferences.getString("ssid", "No configurado");
    String password = preferences.getString("password", "No configurado");

    String ap_ssid = preferences.getString("ap_ssid", "No configurado");
    String ap_password = preferences.getString("ap_password", "No configurado");

    String mqtt_broker = preferences.getString("mqttBroker", "No configurado");
    String mqtt_user = preferences.getString("mqttUser", "No configurado");
    String mqtt_password = preferences.getString("mqttPassword", "No configurado");
    String mqtt_root_topic = preferences.getString("mqttRootTopic", "No configurado");
    bool mqtt_enabled = preferences.getBool("mqttEnabled", false);

    JsonDocument response;
    response["status"] = "success";

    response["wifi"]["ssid"] = ssid;
    response["wifi"]["password"] = password;

    response["ap"]["ap_ssid"] = ap_ssid;
    response["ap"]["ap_password"] = ap_password;

    response["mqtt"]["mqttBroker"] = mqtt_broker;
    response["mqtt"]["mqttUser"] = mqtt_user;
    response["mqtt"]["mqttPassword"] = mqtt_password;
    response["mqtt"]["mqttRootTopic"] = mqtt_root_topic;
    response["mqtt"]["mqttEnabled"] = mqtt_enabled;

    String responseStr;
    serializeJson(response, responseStr);
    client->text(responseStr);
}

void setupWiFi()
{
    WiFi.mode(WIFI_STA);
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");

    if (ssid != "" && password != "")
    {
        WiFi.begin(ssid.c_str(), password.c_str());
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
        {
            delay(100);
        }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.mode(WIFI_AP);
        String ap_ssid = preferences.getString("ap_ssid", "DefaultAP");
        String ap_password = preferences.getString("ap_password", "12345678");
        WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
        Serial.println("Modo AP activado");
    }
    else
    {
        Serial.println("Conexión WiFi establecida en modo Estación");
        Serial.print("Dirección IP obtenida: ");
        Serial.println(WiFi.localIP());   
    }
}
