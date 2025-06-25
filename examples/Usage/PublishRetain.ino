#include <InitMQTT.h>

InitMQTT mqtt;

void setup() {
  Serial.begin(115200);
  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.connect("broker.hivemq.com", 1883);
  mqtt.putRetain("home/led/state", "1");
}

void loop() {
  mqtt.refresh();
}
