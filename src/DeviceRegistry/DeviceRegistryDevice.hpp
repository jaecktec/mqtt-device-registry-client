#include "internals/Datatypes.hpp"

using namespace DeviceRegistryInternals;

class DeviceRegistryDevice{
  friend class DeviceRegistry;
  public: DeviceRegistryDevice(
      boolean sensor,
      String uuid,
      String unit,
      ValueSetFunction valueSetFunction,
      ValueGetFunction getFunction);
  ~DeviceRegistryDevice();

  void setValue(JsonObject& json);
  JsonObject& getValue(JsonObject& objToFill);
  JsonObject& toJson(JsonObject& objToFill);
  const char* getUuid(){return m_uuid.c_str();}
  const char* getUnit(){return m_unit.c_str();}

  void update(){this->m_update = true;};
protected:
  boolean m_sensor;
  boolean m_update;
  String m_uuid;
  String m_unit;
  ValueSetFunction m_setFunction;
  ValueGetFunction m_getFunction;


};

class DeviceRegistrySensor : public DeviceRegistryDevice {
  public:
    DeviceRegistrySensor(String uuid, String unit, ValueGetFunction getFunction);
    ~DeviceRegistrySensor();
  private:
    void sensorUpdateFkt(JsonObject& jsobObj);
};

class DeviceRegistryActor : public DeviceRegistryDevice {
  public:
    DeviceRegistryActor(String uuid, String unit, ValueSetFunction setFunction);
    ~DeviceRegistryActor(){}
};
