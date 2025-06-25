#include <InitMQTT.h>

InitMQTT mqtt;

void onMessage(const char* topic, const char* message) {
  if (String(topic) == "device/led") {
    if (String(message) == "1") {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.setCallback(onMessage);
  mqtt.connect("broker.hivemq.com", 1883);
  mqtt.subscribe("device/led");
}

void loop() {
  mqtt.refresh();
}
