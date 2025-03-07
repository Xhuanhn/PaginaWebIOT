// IoTDevice.cpp
#include "../include/identification-module.h"


IoTDevice::IoTDevice() {
  id = "";
  name = "";
  function = "";
  numVariables = 0;
}

// Constructor para Variable
IoTDevice::Variable::Variable(const String& _name, String _metric,float _min, float _max)
  : name(_name), metric(_metric), min(_min), max(_max) {}

// Constructor para IoTDevice
IoTDevice::IoTDevice(const String& _id, const String& _name, const String& _function)
  : id(_id), name(_name), function(_function) {}

// Método para agregar una variable
bool IoTDevice::addVariable(const String& variableName, String metric,float min, float max) {
  if (numVariables < 5) {
    variables[numVariables] = Variable(variableName,metric, min, max);
    numVariables++;
    return true;
  }
  return false; // No se pudo agregar más variables
}

// Método para obtener la identificación en formato JSON
String IoTDevice::getIdentificationJson() {
  DynamicJsonDocument doc(256); // Tamaño optimizado
  doc["id"] = id;
  doc["name"] = name;
  doc["function"] = function;

  JsonArray vars = doc.createNestedArray("variables");
  for (uint8_t i = 0; i < numVariables; i++) {
    JsonObject var = vars.createNestedObject();
    var["name"] = variables[i].name;
    var["metric"] = variables[i].metric;
    var["min"] = variables[i].min;
    var["max"] = variables[i].max;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

// Función para devolver varios dispositivos inicializados
void IoTDevice::getInitializedDevices(IoTDevice devices[], uint8_t& count) {
  // Inicializar los dispositivos con datos de ejemplo
  devices[0] = IoTDevice("001", "Ultrasonico", "Mide distancia");
  devices[0].addVariable("Distancia", "cm", 20, 450);

  devices[1] = IoTDevice("002", "Presion", "Mide presion");
  devices[1].addVariable("Presion", "bar", 1, 100);

  devices[2] = IoTDevice("003", "flujo", "Mide flujo");
  devices[2].addVariable("Flujo", "L/min", 1, 30);

  // Contar los dispositivos inicializados
  count = 3; // 2 dispositivos inicializados
}