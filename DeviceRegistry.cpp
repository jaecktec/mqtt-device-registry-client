#include "DeviceRegistry.hpp"

using namespace DeviceRegistryInternals;

/** Private *******************************************/

void DeviceRegistry::connect(){
  char* lastWillTopic = this->getTopicBuffer();
  char* lastWillMsg = this->getMessageBuffer();
  char mqttClientSecret[60];

  sprintf(mqttClientSecret, "DeviceRegistry-Client-%d", this->m_unit);
  sprintf(lastWillTopic, TOPIC_UNREGISTER, m_uuid);
  sprintf(lastWillMsg, "{\"id\": \"%s\"}", m_uuid);
  m_pubSubClient.connect(mqttClientSecret, lastWillTopic,0, false, lastWillMsg);
  delay(100);
  this->subscribe();
  m_publish_register = true;
}

void DeviceRegistry::subscribe(){
  char* messageBuffer = this->getMessageBuffer();
  sprintf(messageBuffer, TOPIC_VALUE_SET, m_uuid, "+");
  m_pubSubClient.subscribe(messageBuffer);
  m_pubSubClient.subscribe(TOPIC_UPDATE_REGISTER_REQUEST);
  delay(100);
}

void DeviceRegistry::publishRegister(){
  char* topicBuffer = this->getTopicBuffer();
  sprintf(topicBuffer, TOPIC_REGISTER, m_uuid);
  for(int idx = 0; idx < m_numDevices; idx++){
    StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
    JsonObject& msg = m_devices[idx]->toJson(jsonBuffer.createObject());
    this->publish(topicBuffer, msg);
  }
  delay(100);
}

void DeviceRegistry::publish(const char* topic, ArduinoJson::JsonObject& msg){
  char* messageBuffer = this->getMessageBuffer();
  int msgLength = msg.measureLength() +1;
  msg.printTo(messageBuffer, msgLength);
  while(!m_pubSubClient.publish(topic, (uint8_t*) messageBuffer, msgLength, false)){
    if(!m_pubSubClient.connected()){
      Serial.println("Disconnected while publishing, reconnecting...");
      this->connect();
      delay(500);
    }
  };
}

/** MQTT Callback**************************************/

void DeviceRegistry::mqttCallbackHandler(char* topic,  uint8_t* payload, unsigned int length ){
  if(strcmp(topic, TOPIC_UPDATE_REGISTER_REQUEST) == 0){
    m_publish_register = true;
  }else{
    for( int idx = 0; idx < this->m_numDevices; idx++){
      char* topicBuffer = this->getTopicBuffer();
      DeviceRegistryDevice* device = m_devices[idx];
      sprintf(topicBuffer, TOPIC_VALUE_SET, this->m_uuid, device->getUuid());
      if(strcmp(topic, topicBuffer) == 0){
        StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
        JsonObject& msg = jsonBuffer.parseObject((char*) payload);
        device->setValue(msg);
      }
    }
  }
}

/** Public *******************************************/

void DeviceRegistry::start(){
  m_pubSubClient.setCallback( std::bind(&DeviceRegistry::mqttCallbackHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  this->connect();
}

void DeviceRegistry::handle(){
  m_pubSubClient.loop();
  if(!m_pubSubClient.connected()){
    Serial.println("Reconnecting");
    this->connect();
  }
  if(m_publish_register){
      this->publishRegister();
      m_publish_register = false;
  }

  for( int idx = 0; idx < this->m_numDevices; idx++){
    DeviceRegistryDevice* device = m_devices[idx];
    if(device->m_update){
      device->m_update = false;
      StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
      char* topicBuffer = this->getTopicBuffer();
      sprintf(topicBuffer, TOPIC_VALUE_UPDATE, this->m_uuid, device->getUuid());
      this->publish(topicBuffer, device->getValue(jsonBuffer.createObject()));
    }
  }
}

void DeviceRegistry::addDevice(DeviceRegistryDevice* device){
  DeviceRegistryDevice** tmpList = (DeviceRegistryDevice**) malloc( (m_numDevices + 1) * sizeof( DeviceRegistryDevice*) );
  for(int idx = 0; idx < m_numDevices; idx++){
    tmpList[idx] = m_devices[idx];
  }
  tmpList[m_numDevices] = device;
  free(m_devices);
  m_devices = tmpList;
  m_numDevices++;
}
