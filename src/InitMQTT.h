#pragma once

#include <ESP8266WiFi.h>      // Change this if using ESP32 or others
#include <WiFiClientSecure.h> // or WiFiClient if no SSL needed
#include <PubSubClient.h>

#define INITMQTT_MAX_TOPICS 10
#define INITMQTT_MAX_MSG_LEN 128

#include <functional>

class InitMQTT {
public:
  InitMQTT();

  bool connect(const char* server, uint16_t port, const char* username = nullptr, const char* password = nullptr);

  bool connected();
  void disconnect();

  // Publish methods
  bool put(const char* topic, const char* value);
  bool put(const char* topic, int value);
  bool put(const char* topic, float value);
  bool putRetain(const char* topic, const char* value);
  bool putRetain(const char* topic, int value);
  bool putRetain(const char* topic, float value);

  // Subscribe to a topic explicitly
  bool subscribe(const char* topic);

  // Get last cached message for a topic, or "" if none
  String get(const char* topic);

  // Must call frequently in loop()
  void refresh();

  // User callback: function(topic, message)
  void setCallback(void (*callback)(const char* topic, const char* message));

  // Optional reconnect callback
  void onReconnect(std::function<void()> cb);

  // Optional: Set maximum interval between refresh() calls (ms)
  void setLoopTimeout(unsigned long ms);

  // ✅ New: Set MQTT Last Will and Testament
  void setWill(const char* topic, const char* message, bool retain = true, int qos = 1);

private:
  void mqttCallback(char* topic, byte* payload, unsigned int length);
  bool mqttReconnect();

  WiFiClientSecure _net;
  PubSubClient _client;

  struct TopicCache {
    String topic;
    String lastMessage;
  } _cache[INITMQTT_MAX_TOPICS];

  int _cacheCount = 0;

  void (*userCallback)(const char* topic, const char* message) = nullptr;
  std::function<void()> _reconnectCallback = nullptr;

  const char* _mqttUser = nullptr;
  const char* _mqttPass = nullptr;
  const char* _mqttServer = nullptr;
  uint16_t _mqttPort = 1883;

  unsigned long _lastLoopTime = 0;
  unsigned long _maxLoopInterval = 10000; // default 10 seconds

  // ✅ New: LWT storage
  const char* _willTopic = nullptr;
  const char* _willMessage = nullptr;
  bool _willRetain = true;
  int _willQos = 1;
};
