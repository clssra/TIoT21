#include <MQTTclient.h>
#include <ArduinoJson.h>

const int capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_rec(capacity);
DynamicJsonDocument doc_snd(capacity);
String my_base_topic = "/tiot/21";
const int LED_PIN = 10;
const int TEMP_PIN = A1;
const int INT_LED_PIN = 13;

void setLedValue(const String& topic, const String& subtopic, const String& message){

  DeserializationError err = deserializeJson(doc_rec, message);
  if(err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }

  int value;

  if(doc_rec["e"][0]["n"] == "led"){
    //controlli 
    value = doc_rec["e"][0]["v"];
    digitalWrite(LED_PIN, value);
  }
  
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Bridge.begin();
  mqtt.begin("test.mosquitto.org", 1883);
  mqtt.subscribe(my_base_topic + String("/led"), setLedValue); 

  pinMode(TEMP_PIN, INPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  Bridge.begin();
  digitalWrite(INT_LED_PIN, HIGH);
  

}

String senMlEncode(String res, float v, String unit){
  doc_snd.clear();
  doc_snd["bn"] = "Yun";
  
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

  float T0 = 298;
  float R0 = 100000;
  float B = 4275;
  float a = analogRead(TEMP_PIN);
  float R = ((1023.0 / a) - 1) * R0;
  float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;

  String message = senMlEncode("temperature", T, "Cel");
  mqtt.publish(my_base_topic + String("/temperature"), message);
  mqtt.monitor();
  
  delay(1000);

}
