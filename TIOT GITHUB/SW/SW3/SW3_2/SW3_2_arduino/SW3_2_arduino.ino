#include <MQTTclient.h>
#include <Process.h>
#include <ArduinoJson.h>

#include <Bridge.h>



const int capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_rec(capacity);
DynamicJsonDocument doc_snd(capacity);
String my_base_topic = "iot/21";
const int LED_PIN = 10;
const int TEMP_PIN = A1;
const int INT_LED_PIN = 13;
volatile int x=0;

void setup() {
  //:::::BOH
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 3.2 SW starting");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Bridge.begin();
  Serial.println("Bridge starting");
  mqtt.begin("test.mosquitto.org", 1883);

  pinMode(TEMP_PIN, INPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  digitalWrite(INT_LED_PIN, HIGH);
  String message1 = senMlEncode("temperature", 0, "Cel");
  mqtt.publish(my_base_topic + String("/devices"), message1);
}

String senMlEncode(String res, float v, String unit){
  doc_snd.clear();
  doc_snd["bn"] = "ArduinoTemp";
  
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = millis();
  doc_snd["e"][0]["v"] = v;
  
  if(unit != ""){
    doc_snd["e"][0]["u"] = unit;
  } else {
    doc_snd["e"][0]["u"] = (char*)NULL;
  }
  
  
  String output;
  serializeJson(doc_snd, output);
  return output;
}


void loop() {
  if(millis()-x>100000){
  String message1 = senMlEncode("temperature", 0, "Cel");
  mqtt.publish(my_base_topic + String("/devices"), message1);
  x=millis();
  }
  
  float T0 = 298;
  float R0 = 100000;
  float B = 4275;
  float a = analogRead(TEMP_PIN);
  float R = ((1023.0 / a) - 1) * R0;
  float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;

    
  String message2 = senMlEncode("temperature", T, "Cel");
  mqtt.publish(my_base_topic + String("/temperature"), message2);

  delay(10000);

}
