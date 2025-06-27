# 🧠 InitMQTT — Just MQTT, But Smart™

> Simpler than your ex, smarter than PubSubClient.

**InitMQTT** is a 🔥 firestarter MQTT wrapper for ESP8266/ESP32 that gets rid of the boring boilerplate.
No more `client.setCallback(...)`, `loop()`, `reconnect()`, and `what-do-I-even-do-with-this-payload[]`.
We've done the heavy lifting, so you can vibe and code.

---
## 🚀 Features

* ✅ `connect(server, port, user, pass)` — dead simple
* ✅ `put(topic, value)` — for `String`, `int`, `float`
* ✅ `putRetain(...)` — because memory is good
* ✅ `get(topic)` — cached last message like your browser tabs
* ✅ `subscribe(topic)` — easy, no drama
* ✅ `setCallback(fn)` — get your messages without stress
* ✅ `onReconnect(fn)` — auto-sync like a king
* ✅ `refresh()` — your new `loop()`, but cooler
* 🔒 Uses SSL by default (yes, we care about your security)
* 💡 Works with HiveMQ, Mosquitto, AWS — anything MQTT

> ❗ Built over `PubSubClient`, but we made it feel like you're using an API from 2025.


---
## 🧪 Example: LED Control Vibe Check
---
```
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

## 🤡 Why Not PubSubClient?

Yeah, we use it under the hood — but:

* ❌ Setting up callbacks is messy
* ❌ You have to track subscriptions manually
* ❌ You keep copy-pasting reconnect boilerplate
* ❌ You lose retained messages on reboot (unless you fight it)

> We fixed that. Like an adult.

---

## 🛠️ Usage Playground

Check out [`examples/usages/`](examples/usages/) for:

* `BasicConnect.ino` — Hello MQTT
* `ControlLED.ino` — LED dance party
* `PutRetain.ino` — Because state matters
* `GetLatestMessage.ino` — Read last message like your chat logs
* `WithAuth.ino` — Secure like your password manager
* `OnReconnect.ino` — Sync up when MQTT ghosted you

---

## 👨‍💻 Author

**Siam Rayhan**
[instagram.com/thesiamrayhan](https://instagram.com/thesiamrayhan)

* 💬 DM friendly
* 💻 Built it for my own projects, open-sourcing for the homies
* ☁️ Future-ready IoT libraries are coming 🔜

---

## ⚡ License

**MIT** — Use it, fork it, make millions with it.
Just don’t sell it back to me 🙃

---

## 📢 Final Thoughts

This ain't just another wrapper.
This is **InitMQTT** — where your ESP talks like it knows what it's doing.

> 🧠 Less code, more control. You're welcome.

---
