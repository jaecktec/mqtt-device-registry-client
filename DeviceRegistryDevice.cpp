#include "DeviceRegistryDevice.hpp"

using namespace DeviceRegistryInternals;

DeviceRegistryDevice::DeviceRegistryDevice(boolean sensor,
String uuid,
String unit,
ValueSetFunction valueSetFunction,
ValueGetFunction getFunction)
:m_sensor(sensor)
,m_uuid(uuid)
,m_unit(unit)
,m_setFunction(valueSetFunction)
,m_getFunction(getFunction){

}
DeviceRegistryDevice::~DeviceRegistryDevice(){}

JsonObject& DeviceRegistryDevice::getValue(ArduinoJson::JsonObject &objToFill){
  return this->m_getFunction(objToFill);
}

void DeviceRegistryDevice::setValue(ArduinoJson::JsonObject &json){
  this->m_setFunction(json);
}

JsonObject& DeviceRegistryDevice::toJson(JsonObject& objToFill){
  objToFill["uuid"] = m_uuid;
  objToFill["unit"] = m_unit;
  objToFill["sensor"] = m_sensor;
  return objToFill;
}

DeviceRegistrySensor::DeviceRegistrySensor(String uuid, String unit, ValueGetFunction getFunction)
:DeviceRegistryDevice(true, uuid, unit, NULL, getFunction){
  this->m_setFunction = std::bind(&DeviceRegistrySensor::sensorUpdateFkt, this, std::placeholders::_1);
}

void DeviceRegistrySensor::sensorUpdateFkt(ArduinoJson::JsonObject &jsobObj){
  this->update();
}

DeviceRegistrySensor::~DeviceRegistrySensor(){}

DeviceRegistryActor::DeviceRegistryActor(String uuid, String unit, ValueSetFunction setFunction)
:DeviceRegistryDevice(false, uuid, unit, setFunction, NULL){

}
