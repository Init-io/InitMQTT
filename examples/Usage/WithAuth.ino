#include <InitMQTT.h>

InitMQTT mqtt;

void setup() {
  Serial.begin(115200);
  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.connect("your.mqtt.broker.com", 8883, "mqttUser", "mqttPassword");
  mqtt.put("secured/topic", "hello!");
}

void loop() {
  mqtt.refresh();
}
