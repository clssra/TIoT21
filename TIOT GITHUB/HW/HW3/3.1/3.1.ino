#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>

int INT_LED_PIN = 13;
int LED_PIN = 10;
int TEMP_PIN = A1;

BridgeServer server;
const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_snd(capacity);

void setup() {
  pinMode(TEMP_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  Bridge.begin();
  digitalWrite(INT_LED_PIN, HIGH);
 
  server.listenOnLocalhost();
  server.begin();
  
}



void loop() {
  BridgeClient client = server.accept();

  if(client) {
    process(client);
    client.stop();
  }

  delay(50);

}

void process(BridgeClient client){
  String command = client.readStringUntil('/');
  command.trim();
  
  if(command == "led") {
    int val = client.parseInt();
   // client.println(val);
    if(val == 0 || val == 1){
      digitalWrite(LED_PIN, val);
      printResponse(client, 200, senMlEncode(F("led"), val, F("")));
    } else {
      client.println("Valore inserito non corretto");
    }
  } else if(command == "temperature"){ 
    float T0 = 298;
    float R0 = 100000;
    float B = 4275;
    float a = analogRead(TEMP_PIN);
    float R = ((1023.0 / a) - 1) * R0;
    float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;
    
    printResponse(client, 200, senMlEncode(F("temperature"), T, F("Cel")));
    
    
  } else {
    printResponse(client, 404, "NOT FOUND");
  }
}

String senMlEncode(String res, float v, String unit){
  doc_snd.clear();
  doc_snd["bn"] = "Yun";
//  doc_snd["e"][4];

  //doc_snd["e"][3]["u"] = unit;
  
  if(unit != ""){
    doc_snd["e"][0]["u"] = unit;
  } else {
    doc_snd["e"][0]["u"] = (char*)NULL;
  }
  
  doc_snd["e"][0]["n"] = res;
  doc_snd["e"][0]["t"] = millis();
  doc_snd["e"][0]["v"] = v;
  
  String output;
  serializeJson(doc_snd, output);
  return output;
}



void printResponse(BridgeClient client, int code, String body){
  client.println();
  client.println("Status: " + String(code));
  client.println(F("Content-type: application/json; charset=utf-8"));
  client.println();
  client.println(body);
  
}
