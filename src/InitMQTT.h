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
};

// ========== Implementation ===========

InitMQTT::InitMQTT() : _client(_net) {
  _net.setInsecure(); // Accept all certs (use secure in prod)
  _client.setCallback([this](char* topic, byte* payload, unsigned int length) {
    this->mqttCallback(topic, payload, length);
  });
}

bool InitMQTT::connect(const char* server, uint16_t port, const char* username, const char* password) {
  _mqttServer = server;
  _mqttPort = port;
  _mqttUser = username;
  _mqttPass = password;

  _client.setServer(server, port);

  return mqttReconnect();
}

bool InitMQTT::mqttReconnect() {
  if (_client.connected()) return true;

  Serial.printf("Connecting to MQTT %s:%d ...\n", _mqttServer, _mqttPort);
  if (_mqttUser && _mqttPass) {
    if (_client.connect("InitMQTTClient", _mqttUser, _mqttPass)) {
      Serial.println("MQTT connected with auth");
    } else {
      Serial.printf("MQTT failed rc=%d\n", _client.state());
      return false;
    }
  } else {
    if (_client.connect("InitMQTTClient")) {
      Serial.println("MQTT connected without auth");
    } else {
      Serial.printf("MQTT failed rc=%d\n", _client.state());
      return false;
    }
  }

  // Resubscribe to all cached topics on reconnect
  for (int i = 0; i < _cacheCount; i++) {
    _client.subscribe(_cache[i].topic.c_str());
    Serial.printf("Resubscribed to %s\n", _cache[i].topic.c_str());
  }

  if (_reconnectCallback) _reconnectCallback();

  return true;
}

bool InitMQTT::connected() {
  return _client.connected();
}

void InitMQTT::disconnect() {
  _client.disconnect();
}

bool InitMQTT::put(const char* topic, const char* value) {
  if (!_client.connected()) {
    if (!mqttReconnect()) return false;
  }
  return _client.publish(topic, value);
}

bool InitMQTT::put(const char* topic, int value) {
  return put(topic, String(value).c_str());
}

bool InitMQTT::put(const char* topic, float value) {
  return put(topic, String(value).c_str());
}

bool InitMQTT::putRetain(const char* topic, const char* value) {
  if (!_client.connected()) {
    if (!mqttReconnect()) return false;
  }
  return _client.publish(topic, value, true);
}

bool InitMQTT::putRetain(const char* topic, int value) {
  return putRetain(topic, String(value).c_str());
}

bool InitMQTT::putRetain(const char* topic, float value) {
  return putRetain(topic, String(value).c_str());
}

bool InitMQTT::subscribe(const char* topic) {
  if (!_client.connected()) {
    if (!mqttReconnect()) return false;
  }

  // Check if already subscribed
  for (int i = 0; i < _cacheCount; i++) {
    if (_cache[i].topic == topic) {
      return true; // already subscribed
    }
  }

  if (_cacheCount >= INITMQTT_MAX_TOPICS) {
    Serial.println("InitMQTT: max topic cache reached");
    return false;
  }

  if (_client.subscribe(topic)) {
    _cache[_cacheCount].topic = topic;
    _cache[_cacheCount].lastMessage = "";
    _cacheCount++;
    Serial.printf("Subscribed to topic: %s\n", topic);
    return true;
  }
  return false;
}

String InitMQTT::get(const char* topic) {
  for (int i = 0; i < _cacheCount; i++) {
    if (_cache[i].topic == topic) {
      return _cache[i].lastMessage;
    }
  }
  return "";
}

void InitMQTT::refresh() {
  if (!_client.connected()) {
    mqttReconnect();
  }
  _client.loop();

  if (millis() - _lastLoopTime > _maxLoopInterval) {
    Serial.println("Warning: refresh() may not be called frequently enough!");
  }
  _lastLoopTime = millis();
}

void InitMQTT::setCallback(void (*callback)(const char* topic, const char* message)) {
  userCallback = callback;
}

void InitMQTT::onReconnect(std::function<void()> cb) {
  _reconnectCallback = cb;
}

void InitMQTT::setLoopTimeout(unsigned long ms) {
  _maxLoopInterval = ms;
}

void InitMQTT::mqttCallback(char* topic, byte* payload, unsigned int length) {
  char msg[INITMQTT_MAX_MSG_LEN];
  if (length >= INITMQTT_MAX_MSG_LEN) length = INITMQTT_MAX_MSG_LEN - 1;
  memcpy(msg, payload, length);
  msg[length] = 0;

  for (int i = 0; i < _cacheCount; i++) {
    if (_cache[i].topic == topic) {
      _cache[i].lastMessage = String(msg);
      break;
    }
  }

  if (userCallback) userCallback(topic, msg);
}