#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <Process.h>

int INT_LED_PIN = 13;
int LED_PIN = 10;
int TEMP_PIN = A1;
//const String url = "http://127.0.0.1:8080";

BridgeServer server;
const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_snd(capacity);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 2 Starting");
  pinMode(TEMP_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  Bridge.begin();
  digitalWrite(INT_LED_PIN, HIGH);

  
}



void loop() {


   process();


  delay(2000);

  

}

void process(){

    float T0 = 298;
    float R0 = 100000;
    float B = 4275;
    float a = analogRead(TEMP_PIN);
    float R = ((1023.0 / a) - 1) * R0;
    float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;
    
    String kk = senMlEncode(F("temperature"), T, F("Cel"));

    Serial.println(postRequest(kk));
    
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



int postRequest(String data){
  Process p;
  p.begin("curl");
  
  p.addParameter("-H");
  p.addParameter("Content-Type: application/json");
  p.addParameter("-X");
  p.addParameter("POST");
  p.addParameter("-d");
  p.addParameter(data);
  p.addParameter("http://192.168.1.95:8080/log");
  p.run();
 
 
  return p.exitValue();
}
