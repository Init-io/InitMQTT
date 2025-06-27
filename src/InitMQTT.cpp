#include "InitMQTT.h"

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

  // Set will if defined
  if (_willTopic && _willMessage) {
    _client.setWill(_willTopic, _willMessage, _willRetain, _willQos);
  }

  bool connected = false;
  if (_mqttUser && _mqttPass) {
    connected = _client.connect("InitMQTTClient", _mqttUser, _mqttPass);
  } else {
    connected = _client.connect("InitMQTTClient");
  }

  if (connected) {
    Serial.println("MQTT connected");
  } else {
    Serial.printf("MQTT failed rc=%d\n", _client.state());
    return false;
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

  for (int i = 0; i < _cacheCount; i++) {
    if (_cache[i].topic == topic) {
      return true;
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

void InitMQTT::setWill(const char* topic, const char* message, bool retain, int qos) {
  _willTopic = topic;
  _willMessage = message;
  _willRetain = retain;
  _willQos = qos;
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
