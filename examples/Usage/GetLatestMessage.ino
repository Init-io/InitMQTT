#include <InitMQTT.h>

InitMQTT mqtt;

void onMessage(const char* topic, const char* message) {
  Serial.println("Received.");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.setCallback(onMessage);
  mqtt.connect("broker.hivemq.com", 1883);
  mqtt.subscribe("test/value");
}

void loop() {
  mqtt.refresh();

  String last = mqtt.get("test/value");
  if (last != "") {
    Serial.print("Cached message: ");
    Serial.println(last);
  }

  delay(5000);
}
