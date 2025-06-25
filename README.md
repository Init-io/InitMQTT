---

# 🧠 **InitMQTT — Just MQTT, But Smart™**

> *Simpler than your ex, smarter than PubSubClient.*

**InitMQTT** is a 🔥 firestarter MQTT wrapper for **ESP8266/ESP32** that eliminates the boring boilerplate. No more messing with `client.setCallback(...)`, `loop()`, `reconnect()`, or deciphering `payload[]`.
We did the heavy lifting — so you can **vibe and code**.

---

## 🚀 Features

* ✅ `connect(server, port, user, pass)` — dead simple
* ✅ `put(topic, value)` — supports `String`, `int`, `float`
* ✅ `putRetain(...)` — because memory matters
* ✅ `get(topic)` — cached last message, like your browser tabs
* ✅ `subscribe(topic)` — easy, no drama
* ✅ `setCallback(fn)` — message handling without the stress
* ✅ `onReconnect(fn)` — auto-sync like a king
* ✅ `refresh()` — your new `loop()`, but cooler
* 🔒 SSL by default (yes, we care about your security)
* 💡 Works with **HiveMQ**, **Mosquitto**, **AWS**, or *any MQTT broker*

> ⚙️ *Built on top of `PubSubClient`, but feels like it's from 2025.*

---

## 🧪 Example: LED Control Vibe Check

```cpp
#include <InitMQTT.h>
InitMQTT mqtt;

void onMsg(const char* topic, const char* msg) {
  if (String(topic) == "device/led") {
    digitalWrite(LED_BUILTIN, String(msg) == "1" ? LOW : HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin("YourSSID", "YourPassword");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqtt.setCallback(onMsg);
  mqtt.connect("broker.hivemq.com", 1883);
  mqtt.subscribe("device/led");
}

void loop() {
  mqtt.refresh();
}
```

---

## 🤡 Why Not Just PubSubClient?

Yeah, we use it under the hood. But:

* ❌ Callback setup is clunky
* ❌ You manage subscriptions manually
* ❌ Reconnect boilerplate everywhere
* ❌ Retained messages vanish on reboot (unless you fight for them)

> We fixed that. Like an adult.

---

## 🛠️ Usage Playground

Explore more in [`examples/usages/`](examples/usages/):

* `BasicConnect.ino` — Hello MQTT
* `ControlLED.ino` — LED dance party
* `PutRetain.ino` — Because state matters
* `GetLatestMessage.ino` — Read last message like your chat logs
* `WithAuth.ino` — Secure like your password manager
* `OnReconnect.ino` — Sync up when MQTT ghosted you

---

## 👨‍💻 Author

**Siam Rayhan**
[github.com/iamsiamrayhan](https://github.com/iamsiamrayhan)

* 💬 DM friendly
* 💻 Built it for my own projects — open-sourcing for the homies
* ☁️ Future-ready IoT libraries coming 🔜

---

## ⚡ License

**MIT** — Use it, fork it, make millions with it.
Just don’t sell it back to me 🙃

---

## 📢 Final Thoughts

This ain't just another wrapper.
This is **InitMQTT** — where your ESP talks like it knows what it's doing.

> 🧠 **Less code, more control. You're welcome.**

---
