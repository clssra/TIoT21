
#include <MQTTclient.h>
#include <Process.h>
#include <ArduinoJson.h>
#include <Bridge.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

const int FAN_PIN = 5;
const int TEMP_PIN = A1;
const int INT_LED_PIN = 13;
const int LED_PIN= 6;
const int PIR_PIN = 8;
const int NOISE_PIN = 7;
int x=0;
int noise=0;

const int capacityrec = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 80;


DynamicJsonDocument doc_rec(capacityrec);


String my_base_topic = "iot/21";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 3.4 Starting");

  pinMode(INT_LED_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(NOISE_PIN, INPUT);

  lcd.begin(16,2);
  lcd.setBacklight(255);
  lcd.home();

  delay(5000);
  Bridge.begin();

  attachInterrupt(digitalPinToInterrupt(NOISE_PIN), checkNoise, FALLING);

  digitalWrite(INT_LED_PIN, LOW);
  digitalWrite(INT_LED_PIN, HIGH);
  
  mqtt.begin("test.mosquitto.org", 1883);
  
  mqtt.subscribe(my_base_topic+"/subArd", func_deserialize);
 
  String message = senMlEncode("devices", 0, "NULL");
  mqtt.publish(my_base_topic + String("/devices"), message);

}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.monitor();
  
  String message1 = senMlEncode("devices", 0, "NULL");
  mqtt.publish(my_base_topic + String("/devices"), message1);


  float T0 = 298;
  float R0 = 100000;
  float B = 4275;
  float a = analogRead(TEMP_PIN);
  float R = ((1023.0 / a) - 1) * R0;
  float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;
  
  String message2 = senMlEncode("temperature", T, "Cel");
  mqtt.publish(my_base_topic + String("/pubArd"), message2);
  delay(2000);

  
  int d=checkPresenceNoise();
  message2 = senMlEncode("noise", d, "ON");
  mqtt.publish(my_base_topic + String("/pubArd"), message2); 
  delay(2000);
  
  int b=checkPresenceMovement();
  message2 = senMlEncode("movement", b, "ON");
  mqtt.publish(my_base_topic + String("/pubArd"), message2);
  delay(2000);


  mqtt.monitor();
  delay(2000);
  

}

int checkPresenceNoise(){
  if(noise>500){
  noise=0;
  return 1;
  }
  return 0;
  }

int checkPresenceMovement(){
  if(digitalRead(PIR_PIN)==HIGH){
  return 1;
  }
  return 0;
  }

void checkNoise(){
  Serial.println(noise);
  noise++;
}

void func_deserialize(const String& topic, const String& subtopic, const String& message)
{
    DeserializationError e = deserializeJson(doc_rec, message);
    if(e) Serial.println("Error deserializzation");
    String s=doc_rec["e"][0]["n"];
    Serial.println("ciao"+s);
    
    if (s=="fan"){
      analogWrite(FAN_PIN, float(doc_rec["e"][0]["v"]));
      }
    else if(s=="display"){
      lcd.clear();
      String sentence=doc_rec["e"][0]["v"];
      Serial.println(sentence);
      lcd.print(sentence);
      }
    else if(s=="led"){
      analogWrite(LED_PIN, float(doc_rec["e"][0]["v"]));
      }
    //float val = doc_rec["e"][0]["v"];
    
}

String senMlEncode(String res, float v, String unit){

  Serial.println("invio "+res);
  String output;
  if (res=="devices"){
  output="{\"bn\": \"Arduino\", \"e\": [{\"t\":\""+String(millis())+"\",\"n\": \"pubArd\",\"v\": \""+String(v)+"\"}, {\"t\":\""+String(millis())+"\",\"n\": \"subArd\",\"v\": \""+String(v)+"\"}]}";

  }else{
  output="{\"bn\": \"Arduino\", \"e\": [{\"t\":\""+String(millis())+"\",\"n\": \""+res+"\",\"v\": \""+String(v)+"\", \"u\":\""+unit+"\"}]}";
  //doc_snd["e"][0]["u"] = unit;
 
  }
  return output;

}
