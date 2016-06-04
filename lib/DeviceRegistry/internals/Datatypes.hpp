#ifndef DEVICE_REGISTRY_DATATYPES_HPP
#define DEVICE_REGISTRY_DATATYPES_HPP

#include <Arduino.h>
#include <functional>
#include <ArduinoJson.h>

namespace DeviceRegistryInternals {

  typedef std::function<void(JsonObject&)> ValueSetFunction;

  typedef std::function<JsonObject&(JsonObject&)> ValueGetFunction;

  typedef std::function<void(JsonObject&)> OnMessageHandler;

}

#endif
