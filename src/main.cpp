#include <BeeNeXT.h>
#include "lcd_iot.h"

#define SSID              "------"
#define PASSWORD          "------"

#define MQTT_HOST         "broker.hivemq.com"
#define MQTT_PORT         1883
#define MQTT_USER         ""
#define MQTT_PASSWORD     ""

void setup() {
  Serial.begin(115200); Serial.println();

  WIFI::begin(SSID, PASSWORD);   /* เชื่อมต่อไวไฟ */

  lcd_iot::begin(MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

  static SoftTimer timer_sensor;
  timer_sensor.setInterval(1000,[](){             /* ตั้งเวลาเป็นช่วงๆ ในการอ่าน sensor */
    float temp  = random(10000)/100.0;            /* จำลองโดยใช้สุ่มตัวเลข  ใช้งานจริง ให้อ่านตาม sensor ที่ใช้ */
    float humid = random(10000)/100.0;            /* จำลองโดยใช้สุ่มตัวเลข  ใช้งานจริง ให้อ่านตาม sensor ที่ใช้ */
    Serial.printf("[Temp] %.2f ; [Humid] %.2f \n", temp, humid);

    if(lcd_iot::connected()) {                    /* เมื่อ เชื่อมต่อ mqtt สำเร็จแล้ว */
      lcd_iot::print("/mylcd/iot/temp",  temp);   /* print ค่า temp ไปยัง lcd ทาง iot โดยส่งไปที่ topic "/mylcd/iot/temp" */
      lcd_iot::print("/mylcd/iot/humid", humid);  /* print ค่า humid ไปยัง lcd ทาง iot โดยส่งไปที่ topic "/mylcd/iot/humid" */
    }
  });

  /* หากต้องการรับค่าจากจอ lcd_iot */
  // lcd_iot::subscribe("/mylcd/iot/switch01");
  // lcd_iot::onData([](String topic, String message){
  //   if(topic == "/mylcd/iot/switch01"){
  //     bool switch_state = message.toInt();
  //   }
  // });

}

void loop() {
  BeeNeXT.update();
}

