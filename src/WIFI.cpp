#include "lcd_iot.h"

namespace WIFI {
  void begin(String ssid, String password);
}

void WiFiNoOpCbk() {}
WIFI_CONNECTED() __attribute__((weak, alias("WiFiNoOpCbk")));
WIFI_DISCONNECTED() __attribute__((weak, alias("WiFiNoOpCbk")));

void WIFI::begin(String ssid, String password){
  WiFi.begin(ssid, password);
  WiFi.onEvent([](arduino_event_t *event){
    Serial.print("[WiFi] connected. IP : ");
    Serial.println(WiFi.localIP());
    WiFiOnConnected();
  }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.onEvent([](arduino_event_t *event){ 
    Serial.println("[WiFi] disconnected.");
    WiFiOnDisconnected(); 
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

