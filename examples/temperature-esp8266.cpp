#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <DeviceRegistry.hpp>

#include <DHT.h>

const char* SSID = "***";
const char* PASSWORD = "***";
const char* MQTT_SERVER = "**";

WiFiClient wclient;
PubSubClient client(MQTT_SERVER, 1883 ,wclient);

DHT dht(4, DHT22);

JsonObject& getTemperature(JsonObject& result){
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Error reading temperature!");
  } else {
    result["value"] = t;
  }
  return result;
}

JsonObject& getHumidity(JsonObject& obj){
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Error reading humidity!");
  } else {
    obj["value"] = h;
  }
  return obj;
}

DeviceRegistrySensor temperature("tmp-001", "°C", getTemperature);
DeviceRegistrySensor humidity("hum-002", "%", getHumidity);
DeviceRegistry DeviceRegistry("Node-001", client);

boolean connectToWifi(){

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    delay(1000);

    Serial.printf("\n\n\nSetup WIFI, connecting to %s...", SSID);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if(WiFi.status() == WL_NO_SSID_AVAIL){
        break;
      }
    }

    return WiFi.status() == WL_NO_SSID_AVAIL;
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println(".");

  if(!connectToWifi()){
    Serial.println("SSID Not available");
    Serial.println("Restarting");
    ESP.restart();
  }else{
    DeviceRegistry.addDevice(&temperature);
    DeviceRegistry.addDevice(&humidity);
    DeviceRegistry.addDevice(&systemInfo);
    DeviceRegistry.addDevice(&serialOut);
    DeviceRegistry.start();
  }

}

uint16_t counter = 0;

void loop() {
  DeviceRegistry.handle();
  if(counter < 1000){
    counter++;
    delay(10);
  }else{
    counter = 0;
    temperature.update();
    humidity.update();
    systemInfo.update();
  }
}
