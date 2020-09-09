#include <MQTTclient.h>
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

void func_deserialize(const String& topic, const String& subtopic, const
String& message)
{
    DeserializationError e = deserializeJson(doc_rec, message);
    if(e) Serial.println("Error deserializzation");
    float val = doc_rec["e"][0]["v"];
    digitalWrite(LED_PIN, val);
}

void setup() {
  //:::::BOH
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 3.2 SW starting");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Bridge.begin();
  mqtt.begin("test.mosquitto.org", 1883);
  
  pinMode(TEMP_PIN, INPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  digitalWrite(INT_LED_PIN, HIGH);

  mqtt.subscribe(my_base_topic+"/led", func_deserialize);
  
  String message1 = senMlEncode("led", 0, "On");
  mqtt.publish(my_base_topic + String("/devices"), message1);
}

String senMlEncode(String res, float v, String unit){
  doc_snd.clear();
  doc_snd["bn"] = "ArduinoLed";

  
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
  String message1 = senMlEncode("led", 0, "On");
  mqtt.publish(my_base_topic + String("/devices"), message1);
  x=millis();
  }

  mqtt.monitor();
}
