
#ifndef DEVICE_REGISTRY_HPP
#define DEVICE_REGISTRY_HPP

#define MQTT_TOPIC_MAX_LENGTH 60

#include "DeviceRegistryDevice.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

class DeviceRegistry {
    public:
    DeviceRegistry(const char* nodeUuid, PubSubClient& client):m_uuid(nodeUuid),m_numDevices(0),m_pubSubClient(client){}
    ~DeviceRegistry(){}


    void addDevice(DeviceRegistryDevice* device);
    void start();
    void handle();
  private:
    const char* TOPIC_REGISTER = "dr/register/%s";
    const char* TOPIC_UNREGISTER = "dr/unregister/%s";
    const char* TOPIC_VALUE_UPDATE = "dr/device/%s/%s";
    const char* TOPIC_VALUE_SET = "dr/device/%s/%s/set";
    const char* TOPIC_UPDATE_REGISTER_REQUEST = "dr/register/update";
    void publish(const char* topic, JsonObject& msg);
    void publishRegister();
    void publishValueUpdate(const char* topic, String deviceId, JsonObject& msg);
    void connect();
    void subscribe();
    void mqttCallbackHandler(char* topic,  uint8_t* payload, unsigned int payloadLength );

    char* getTopicBuffer(){
        //std::fill_n(m_topicBuffer, MQTT_TOPIC_MAX_LENGTH, 0);
        return this->m_topicBuffer;
    }

    char* getMessageBuffer(){
        //std::fill_n(m_messageBuffer, MQTT_MAX_PACKET_SIZE, 0);
        return this->m_messageBuffer;
    }

    const char* m_uuid;
    char m_topicBuffer[MQTT_TOPIC_MAX_LENGTH];
    char m_messageBuffer[MQTT_MAX_PACKET_SIZE];
    bool m_publish_register;
    PubSubClient& m_pubSubClient;
    DeviceRegistryDevice** m_devices;
    int m_numDevices;
  };

#endif
