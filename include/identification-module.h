// IoTDevice.h
#ifndef IDENTIFICATION_MODULE_H
#define IDENTIFICATION_MODULE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class IoTDevice {
  public:
    struct Variable {
      String name;
      String metric;
      float min;
      float max;

      Variable() : name(""), metric(""), min(0), max(0) {}
      // Constructor que toma los parámetros necesarios
      Variable(const String& _name, String _metric,float _min, float _max);
    };

  private:
    String id;
    String name;
    String function;
    Variable variables[5];  // Número máximo de variables
    uint8_t numVariables = 0;

  public:
    IoTDevice();

    IoTDevice(const String& _id, const String& _name, const String& _function);
    bool addVariable(const String& variableName, String metric, float min, float max);
    String getIdentificationJson();

    // Función para devolver varios dispositivos inicializados
    static void getInitializedDevices(IoTDevice devices[], uint8_t& count);
};


#endif
