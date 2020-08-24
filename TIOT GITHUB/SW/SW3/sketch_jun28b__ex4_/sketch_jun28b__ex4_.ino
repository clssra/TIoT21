#include <MQTTclient.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <Process.h>

#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

const int capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(5) + 40;
DynamicJsonDocument doc_rec(capacity);
DynamicJsonDocument doc_snd(capacity);
BridgeServer server;
String my_base_topic = "/iot/21";
const int LED_PIN = 10;
const int FAN_PIN = 5;
const int TEMP_PIN = A1;
const int INT_LED_PIN = 13;

String pub[] = {"temperature", "noise", "presence"};
String sub[] = {"led", "fan", "lcd"};

void setValues(const String& topic, const String& subtopic, const String& message){

  DeserializationError err = deserializeJson(doc_rec, message);
  if(err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }

  int value;
  //char* valueLcd;
  String output;
  //serializeJson(doc_snd, output);
  //Serial.println(output);
  
  if(doc_rec["e"][0]["n"] == "led"){
    //controlli 
    value = doc_rec["e"][0]["v"];
    digitalWrite(LED_PIN, value);
  } else if(doc_rec["e"][0]["n"] == "fan"){
    value =  doc_rec["e"][0]["v"];
    digitalWrite(FAN_PIN, value);
  } 
  /*
  else if(doc_rec["e"][0]["n"] == "lcd"){
    strcpy(valueLcd, doc_rec["e"][0]["v"]);
    lcd.clear();
    lcd.home();
    lcd.print(valueLcd);
  }
  */
}

String senMlEncode2(String res[], float v, String unit, String type){
  doc_snd.clear();
  
  doc_snd["bn"] = "arduino" + type;

  for(int i=0; i<3; i++){
      doc_snd["e"][i]["n"] = res;
      doc_snd["e"][i]["t"] = millis();
      doc_snd["e"][i]["v"] = v;

      if(unit != ""){
        doc_snd["e"][i]["u"] = unit;
      } else {
        doc_snd["e"][i]["u"] = (char*)NULL;
      }
  }
  


  
  
  

  /*
 
  doc_snd["ID"] = "arduino" + type;
  for(int i=0; i<3; i++){
    doc_snd["EndPoints"][i] = res[i];
    doc_snd["AvailResurces"][i] = res[i]; 
  }
  /*
  doc_snd["EndPoints"][0] = res[0];
  doc_snd["EndPoints"][1] = res[1];
  doc_snd["EndPoints"][2] = res[2];
  
  doc_snd["AvailResurces"][0]["1"] = "Temperature";
  doc_snd["AvailResurces"][0]["2"] = "Temperature";
  doc_snd["AvailResurces"][0]["3"] = "Temperature";
 
  */
  
  
  String output;
  
  serializeJson(doc_snd, output);

  Serial.println(output);
  return output;
}



String senMlEncode(String res[], float v, String unit, String type){
  doc_snd.clear();
  /*
  doc_snd["bn"] = "Yun";
  
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = millis();
  doc_snd["e"][0]["v"] = v;
  
  if(unit != ""){
    doc_snd["e"][0]["u"] = unit;
  } else {
    doc_snd["e"][0]["u"] = (char*)NULL;
  }
  */
 
  doc_snd["ID"] = "arduino" + type;
  for(int i=0; i<3; i++){
    doc_snd["EndPoints"][i] = res[i];
    doc_snd["AvailResurces"][i] = res[i]; 
  }
  /*
  doc_snd["EndPoints"][0] = res[0];
  doc_snd["EndPoints"][1] = res[1];
  doc_snd["EndPoints"][2] = res[2];
  
  doc_snd["AvailResurces"][0]["1"] = "Temperature";
  doc_snd["AvailResurces"][0]["2"] = "Temperature";
  doc_snd["AvailResurces"][0]["3"] = "Temperature";
 
  */
  
  
  String output;
  
  serializeJson(doc_snd, output);

  Serial.println(output);
  return output;
}



void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Bridge.begin();
  mqtt.begin("test.mosquitto.org", 1883);
  for(int i=0; i<3; i++){
    mqtt.subscribe(my_base_topic + sub[i], setValues); 
    mqtt.subscribe(my_base_topic + sub[i], setValues);
    mqtt.subscribe(my_base_topic + sub[i], setValues);
  }
  pinMode(FAN_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  Bridge.begin();
  digitalWrite(INT_LED_PIN, HIGH);
  

}



void loop() {


  float T0 = 298;
  float R0 = 100000;
  float B = 4275;
  float a = analogRead(TEMP_PIN);
  float R = ((1023.0 / a) - 1) * R0;
  float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;
  bool flag_pir = true, flag_noise = false;

  String pubMsg = senMlEncode2(pub, 3243, "ciaooo", "Pub");
  String subMsg = senMlEncode2(sub, 32432, "cio", "Sub");
  float val[] = {T, flag_noise, flag_pir};
  
//  Serial.println(postRequest(pubMsg));
  
  mqtt.monitor();
 
  mqtt.publish(String("iot/21/devices"), pubMsg);
  mqtt.publish(String("iot/21/devices"), subMsg);

  for(int i=0; i<3; i++){
    mqtt.publish("iot/21/" + pub[i], val[i]);
  }

 // mqtt.publish("iot/21/temperature", T);
  
  delay(1000);

}

/*

int postRequest(String data){
  Process p;
  p.begin("curl");
  
  p.addParameter("-H");
  p.addParameter("Content-Type: application/json");
  p.addParameter("-X");
  p.addParameter("POST");
  p.addParameter("-d");
  p.addParameter(data);
  p.addParameter("http://192.168.1.95:8080/devices");
  p.run();
 
 
  
 // p.begin("curl -H \"Content-Type: application/json\" -X POST -d '{\"json_string\":\"yeyyyy\"}' http://127.0.0.1:8080");
 // Serial.println("-H \"Content-Type: application/json\" -X POST -d '{\"json_string\":\"yeyyyy\"}' http://127.0.0.1:8080");

  return p.exitValue();
}
*/
