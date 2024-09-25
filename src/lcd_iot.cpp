#include <BeeNeXT.h>
#include "lcd_iot.h"

namespace lcd_iot {
  void begin(String mqtt_host, uint16_t mqtt_port, String mqtt_user, String mqtt_password);
  void print(String topic, String message);
  void print(String topic,  float value, int decimalPlaces);
  void print(String topic,  double value, int decimalPlaces);
  void print(String topic, int value);
  void subscribe(String topic);
  void onData(void(*fc_ondata_cb)(String topic, String message));
  void (*getOnDataCallback())(String topic, String message);
  bool connected();
  void disconnect();
}

void(*_lcd_fc_onData)(String topic, String message) = NULL;

void lcd_iot::begin(String mqtt_host, uint16_t mqtt_port, String mqtt_user, String mqtt_password){
  SmartMQTT::setServer(mqtt_host, mqtt_port, mqtt_user, mqtt_password);
}

void lcd_iot::print(String topic, String message){
  SmartMQTT::publish(topic, message);
}

void lcd_iot::print(String topic, int value){
  Serial.println("[int]");
  lcd_iot::print(topic, String(value));
}

void lcd_iot::print(String topic, float value, int decimalPlaces){
  Serial.println("[float]");
  lcd_iot::print(topic, String(value, decimalPlaces));
}

void lcd_iot::print(String topic, double value, int decimalPlaces){
  Serial.println("[double]");
  lcd_iot::print(topic, String(value, decimalPlaces));
}

void lcd_iot::subscribe(String topic){
  SmartMQTT::subscribe(topic);
}

void lcd_iot::onData(void(*fc_ondata_cb)(String topic, String message)){
  _lcd_fc_onData = fc_ondata_cb;
}

void (*lcd_iot::getOnDataCallback())(String topic, String message) {
  return _lcd_fc_onData;
}

bool lcd_iot::connected(){
  return SmartMQTT::connected();
}

void lcd_iot::disconnect(){
  SmartMQTT::disconnect();
}
