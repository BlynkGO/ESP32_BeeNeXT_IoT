#ifndef __LCD_IOT_H__
#define __LCD_IOT_H__

#include <WiFi.h>
#include <BeeNeXT.h>
#include <PubSubClient.h>

#define WIFI_CONNECTED()    void WiFiOnConnected()
#define WIFI_DISCONNECTED() void WiFiOnDisconnected()
#ifdef __cplusplus
extern "C" {
#endif
  void WiFiNoOpCbk();
  WIFI_CONNECTED();
  WIFI_DISCONNECTED();
#ifdef __cplusplus
}
#endif

namespace WIFI {
  void begin(String ssid, String password);
}

namespace SmartMQTT {
  void setServer(String mqtt_host, uint16_t mqtt_port, String mqtt_user="", String mqtt_password="");
  void publish(String topic, String message, boolean retained=false);
  void subscribe(String topic);
  void onData(void(*fc_ondata_cb)(String topic, String message));
  bool connected();
  void disconnect();
}

namespace lcd_iot {
  void begin(String mqtt_host, uint16_t mqtt_port, String mqtt_user="", String mqtt_password="");
  void print(String topic, String message);
  void print(String topic,  float value, int decimalPlaces=2);
  void print(String topic,  double value, int decimalPlaces=2);
  void print(String topic, int value);
  void subscribe(String topic);
  void onData(void(*fc_ondata_cb)(String topic, String message));
  void (*getOnDataCallback())(String topic, String message);
  bool connected();
  void disconnect();
}



#endif //__LCD_IOT_H__