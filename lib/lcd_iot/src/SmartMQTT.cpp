#include "PubSubClient.h"
#include "lcd_iot.h"

#if defined(ESP8266)
  #define MQTT_BUFFER_SIZE      (5*1024)
#elif defined(ESP32)
  #define MQTT_BUFFER_SIZE      (30*1024)
#endif

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined (ESP32)
  #include <WiFi.h>
#endif

#include <BeeNeXT.h>
#include <vector>

WiFiClient _mqtt_client;
PubSubClient _mqtt(_mqtt_client);
std::vector<String> mqtt_subscribe_topics;
void(*fc_onData)(String topic, String message) = NULL;

String _mqtt_host;
uint16_t _mqtt_port;
String _mqtt_user;
String _mqtt_password;

namespace SmartMQTT {
  void setServer(String mqtt_host, uint16_t mqtt_port, String mqtt_user, String mqtt_password);
  void publish(String topic, String message, boolean retained);
  void subscribe(String topic);
  void onData(void(*fc_ondata_cb)(String topic, String message));
  bool connected();
  void disconnect();
}

void SmartMQTT::setServer(String mqtt_host, uint16_t mqtt_port, String mqtt_user, String mqtt_password) {
  mqtt_subscribe_topics.clear();

  _mqtt_host = mqtt_host;
  _mqtt_port = mqtt_port;
  _mqtt_user = mqtt_user;
  _mqtt_password = mqtt_password;

  if ( !_mqtt.setBufferSize(MQTT_BUFFER_SIZE) ) {
    Serial.println("[MQTT] can't alloc");
  }
  _mqtt.setServer( _mqtt_host.c_str(), _mqtt_port);
  _mqtt.setCallback([](char* topic, byte * data, unsigned int data_len) {
    std::unique_ptr<char[]> message;

#if defined(ESP32)
    if (psramFound()) {
      message.reset(reinterpret_cast<char*>(ps_malloc(data_len + 1)));
    } else {
      message.reset(new char[data_len + 1]);
    }
#else
    message.reset(new char[data_len + 1]);
#endif

    if(!message) {
      Serial.println("[MQTT] Failed to allocate memory");
      return;
    }

    memcpy(message.get(), data, data_len); message[data_len] = '\0';
    if (fc_onData) {
      fc_onData(topic, message.get());
    }

    void (*lcd_onData)(String, String) = lcd_iot::getOnDataCallback();
    if( lcd_onData != NULL ) {
      lcd_onData(topic, message.get());
    }
  });

  static SoftTimer _timer_mqtt;
  _timer_mqtt.setInterval(1, []() {
    _mqtt.loop();
  });

  static SoftTimer _timer_mqtt_connection;
  _timer_mqtt_connection.setInterval(5000, []() {
    if (!WiFi.isConnected()) return;
    if (_mqtt.connected()) return;

    Serial.printf("[MQTT] connecting to '%s:%d' ...\n", _mqtt_host.c_str(), _mqtt_port);
    String mqtt_client_id = "MQTT_CID_" + String(random(0xFFFF), HEX);

    bool ret = false;
    ret = (_mqtt_user == "") ? _mqtt.connect(mqtt_client_id.c_str()) : _mqtt.connect(mqtt_client_id.c_str(), _mqtt_user.c_str(), _mqtt_password.c_str());
    if (ret) {
      Serial.println("[MQTT] connected & subscribe");
      // จะ subscribe อะไร ให้วางตรงนี้ ---------
      if (mqtt_subscribe_topics.size()) {
        for (int i = 0; i < mqtt_subscribe_topics.size(); i++) {
          Serial.printf("[MQTT] subcribe : %s\n", mqtt_subscribe_topics[i].c_str());
          _mqtt.subscribe(mqtt_subscribe_topics[i].c_str());
        }
      }
      // subscribe แบบ กำหนด QOS
      // uint8_t QOS = 0;   // 0, 1, 2
      // _mqtt.subscribe("BlynkGO/topic1", QOS);

    } else {
      Serial.println("[MQTT] ERR :  connection fail!");
    }
  }, true);
}

void SmartMQTT::subscribe(String topic) {
  mqtt_subscribe_topics.push_back(topic);
  if (_mqtt.connected()) {
    Serial.printf("[MQTT] subcribe : %s\n", topic.c_str());
    _mqtt.subscribe(topic.c_str());
  }
}

void SmartMQTT::onData(void(*fc)(String topic, String message)) {
  fc_onData = fc;
}

void SmartMQTT::publish(String topic, String message, boolean retained) {
  // Serial.println("[MQTT] publish");
  _mqtt.publish(topic.c_str(), message.c_str(),retained);
}

bool SmartMQTT::connected(){
  return _mqtt.connected();
}

void SmartMQTT::disconnect(){
  _mqtt.disconnect();
}
