#include <InitMQTT.h>

InitMQTT mqtt;

void onMessage(const char* topic, const char* message) {
  Serial.printf("Message on %s: %s\n", topic, message);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  mqtt.setCallback(onMessage);
  mqtt.connect("broker.hivemq.com", 1883);
  mqtt.subscribe("test/topic");
}

void loop() {
  mqtt.refresh();
}
