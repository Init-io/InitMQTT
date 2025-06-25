#include <InitMQTT.h>

InitMQTT mqtt;

void setup() {
  Serial.begin(115200);
  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.onReconnect([]() {
    Serial.println("MQTT reconnected!");
    mqtt.subscribe("device/led");
  });

  mqtt.connect("broker.hivemq.com", 1883);
}

void loop() {
  mqtt.refresh();
}
