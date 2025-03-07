#include "websocket.h"
#include "config.h"
#include "identification-module.h"
#include "flow-sensor.h"
#include "pressure-sensor.h"
#include "ultrasonic-sensor.h"

void sendResponse(AsyncWebSocketClient *client, const String &status, const String &message)
{
    JsonDocument response;
    response["status"] = status;
    response["message"] = message;
    String responseStr;
    serializeJson(response, responseStr);
    client->text(responseStr); 
}

void broadcastSensorValues() {
    JsonDocument doc;
    JsonObject sensors = doc.createNestedObject("sensors");

    sensors["flow"] = readFlow();
    sensors["pressure"] = readPressure();
    sensors["ultrasonic"] = readUltrasonic();

    String json;
    serializeJson(doc, json);

    Serial.println("Enviando datos WebSocket: " + json); 
    ws.textAll(json);  
}


void sendJson(AsyncWebSocketClient *client, JsonDocument doc)
{
    String responseStr;
    serializeJson(doc, responseStr);
    client->text(responseStr); 
}

void setConfig(bool mqttUpdated, JsonObject config, AsyncWebSocketClient *client)
{
    if (config.containsKey("mqttBroker"))
    {
        mqtt_broker = config["mqttBroker"].as<String>();
        preferences.putString("mqttBroker", mqtt_broker);
        mqttUpdated = true;
    }
    if (config.containsKey("mqttUser"))
    {
        mqtt_user = config["mqttUser"].as<String>();
        preferences.putString("mqttUser", mqtt_user);
        mqttUpdated = true;
    }
    if (config.containsKey("mqttPassword"))
    {
        mqtt_password = config["mqttPassword"].as<String>();
        preferences.putString("mqttPassword", mqtt_password);
        mqttUpdated = true;
    }
    if (config.containsKey("mqttRootTopic"))
    {
        mqtt_root_topic = config["mqttRootTopic"].as<String>();
        preferences.putString("mqttRootTopic", mqtt_root_topic);
        mqttUpdated = true;
    }
    if (config.containsKey("mqttEnabled"))
    {
        mqtt_enabled = config["mqttEnabled"].as<bool>();
        preferences.putBool("mqttEnabled", mqtt_enabled);
        Serial.println("Here");
        mqttUpdated = true;
    }
    if (mqttUpdated)
    {
        sendResponse(client, "success", "Configuración MQTT recibida y guardada.");
    }
    if (config.containsKey("newSSID") && config.containsKey("newPassword"))
    {
        preferences.putString("ssid", config["newSSID"].as<String>());
        preferences.putString("password", config["newPassword"].as<String>());
        WiFi.begin(config["newSSID"].as<String>().c_str(), config["newPassword"].as<String>().c_str());
        sendResponse(client, "success", "Nuevo SSID y contraseña de WiFi configurados.");
    }
    if (config.containsKey("newAP_SSID") && config.containsKey("newAP_Password"))
    {
        preferences.putString("ap_ssid", config["newAP_SSID"].as<String>());
        preferences.putString("ap_password", config["newAP_Password"].as<String>());
        WiFi.softAP(config["newAP_SSID"].as<String>().c_str(), config["newAP_Password"].as<String>().c_str());
        sendResponse(client, "success", "Nuevo SSID y contraseña de AP configurados.");
    }
    if (config.containsKey("restart") && config["restart"].as<bool>())
    {
        ESP.restart();
    }
    if (config.containsKey("checkWiFi"))
    {
        checkWiFiStatus(client);
    }
}

IoTDevice devices[3];
uint8_t devicesCount = 0;

String getSensors(JsonDocument& doc)
{
    JsonArray sensorsArray = doc.to<JsonArray>();

    IoTDevice::getInitializedDevices(devices, devicesCount);

    for (uint8_t i = 0; i < devicesCount; i++)
    {
        JsonDocument sensorDoc;
        DeserializationError error = deserializeJson(sensorDoc, devices[i].getIdentificationJson());

        if (!error)
        {
            sensorsArray.add(sensorDoc.as<JsonObject>());
        }
    }

    String jsonString;
    serializeJson(doc, jsonString);

    return jsonString;
}

void onWsEvent(AsyncWebSocket *ws, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{
    if (type != WS_EVT_DATA)
        return;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);

    if (error)
    {
        Serial.println("Error de deserialización JSON");
        return;
    }

    const String command = doc["command"].as<String>();

    if (command.isEmpty())
        return;

    JsonObject config = doc["value"];
    bool mqttUpdated = false;

    switch (command.charAt(0))
    {
    case 's': // set_config
        if (command == "set_config")
        {
            setConfig(mqttUpdated, config, client);
        }
        break;

    case 'g': // get_config
        if (command == "get_config")
        {
            sendFullConfig(client);
        }
        else if (command == "get_sensors")
        {
            JsonDocument json;
            getSensors(json);
            sendJson(client,json);
        }
        else if (command == "get_ultrasonic")
        {
            int distance = readUltrasonic();
            sendResponse(client,"success",String(distance));
        }
        else if (command == "get_pressure")
        {
            float pressure = readPressure();
            sendResponse(client,"success",String(pressure));
        }
        else if (command == "get_flow")
        {
            float flowRate = readFlow();
            sendResponse(client,"success",String(flowRate));
        }
        else{
            sendResponse(client,"Unknown","Comando desconocido");
        }
        break;

    case 'u': // update_mqtt_credentials
        if (command == "update_mqtt_credentials")
        {
            if (config.containsKey("mqttUser") && config.containsKey("mqttPassword"))
            {
                mqtt_user = config["mqttUser"].as<String>();
                mqtt_password = config["mqttPassword"].as<String>();
                preferences.putString("mqttUser", mqtt_user);
                preferences.putString("mqttPassword", mqtt_password);
                mqttUpdated = true;
            }

            if (mqttUpdated)
            {
                sendResponse(client, "success", "MQTT credentials updated");
            }
            else
            {
                sendResponse(client, "error", "Failed to update MQTT credentials");
            }
        }
        break;

    default:
        Serial.println("Comando no reconocido");
        sendResponse(client, "error", "Comando no reconocido");
        break;
    }
}