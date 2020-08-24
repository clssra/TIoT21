#include <MQTTclient.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <Process.h>

const int capacity2 = JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(10);
const int capacity = JSON_OBJECT_SIZE(2);
DynamicJsonDocument doc_rec(capacity);
DynamicJsonDocument doc_snd(capacity2);
BridgeServer server;
String my_base_topic = "iot/21";
const int LED_PIN = 10;
const int TEMP_PIN = A1;
const int INT_LED_PIN = 13;
const int PIR_PIN = 8;
const int NOISE_PIN = 7;

String sub[] = {"led", "fan", "lcd"};
String pub[] = {"temp", "noise", "pres"};

//pir
volatile bool presence_pir = false;
volatile unsigned long  prev_time_pir = 0;
const unsigned long timeout_pir = 10 * 1e03; //cambia in ....
volatile unsigned long time_pir;
volatile int count_pir = 0;
volatile bool flag_pir = false;

//noise
const unsigned long sound_interval = 2 * 1e03; //cambia in 60 * 1e03
const unsigned long timeout_sound = 10 * 1e03; //cambia in 3600 * 1e03
volatile int timez[10];
volatile int sum = 0;
volatile int time_sound_count=0;
volatile int count_noise = 0;
volatile bool presence_noise = false;
volatile unsigned long prev_time_sound_interval = 0;
volatile unsigned long prev_time_sound = 0;
volatile unsigned long time_sound_interval;
volatile unsigned long time_sound;
volatile bool flag_sound = false;


// -----------------CHECK PRESENCE-------------------------------------

void checkPresence(){  
  int val = digitalRead(PIR_PIN);
  if(val == HIGH){
     presence_pir = true;
  } else {
    presence_pir = false;
  }
}

void checkPresence2(){
  count_noise++;
}

// ---------- CHECK TIME INTERVAL ---------------------------------

void checkPir(){

  checkPresence();

  time_pir = millis();
  
  if(time_pir - prev_time_pir >= timeout_pir || presence_pir == true ){ //da cambiare in 30 minn  

    if(time_pir - prev_time_pir >= timeout_pir){
      flag_pir = false;
    }

    prev_time_pir = time_pir;

    if(presence_pir == true){
      flag_pir = true;
    }
  }
}

void checkSoundInterval(){

  time_sound_interval = millis();
   
  if(time_sound_interval - prev_time_sound_interval >= sound_interval){ 
     
      timez[time_sound_count] = count_noise; //salvo in cas vettore # eventi in quel minuto
      
      count_noise=0;

      for(int i=0; i<10; i++){
        sum += timez[i];
      }
      
      if(sum>20){ //somma degli eventi degli ultimi 10 min (in media se c'è qualcuno "eventi" al secondo circa 100)
        presence_noise = true;
      } else {
        presence_noise = false;
      }
      
     sum = 0;
     prev_time_sound_interval = time_sound_interval;

     time_sound_count++;

     //aka 10 min passati --> vado a sostituire # eventi in indice 0
     if(time_sound_count==10){
        time_sound_count = 0;
      }
      
  }
}

void checkSound(){

  time_sound = millis();
  
  checkSoundInterval();
  
  if(time_sound - prev_time_sound >= timeout_sound || presence_noise == true){ //da cambiare in 60 minn

    if(time_sound - prev_time_sound >= time_sound && presence_noise == false){
       flag_sound=false;
    }

    prev_time_sound = time_sound;

    if(presence_noise == true){
      flag_sound = true;
    }
    
  }
}


void setLedValue(const String& topic, const String& subtopic, const String& message){
 // doc_rec.clear();
 Serial.println("aaaaaaaaaaaaa");
  DeserializationError err = deserializeJson(doc_rec, message);
  if(err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }
  
  int value;
 // String output;
  //serializeJson(doc_snd, output);
  //Serial.println(output);
  
  if(doc_rec["tipo"] == "led"){
    Serial.println("aaaaaaaaaaaIUGFGLIFDIYFGLFTaa");
    //controlli 
    value = doc_rec["val"];
    digitalWrite(LED_PIN, value);
   
  }

  doc_rec.clear();
}

String senMlEncode(String res[], float v, String type){
  doc_snd.clear();
  
  doc_snd["bn"] = "arduino" + type;

  
  for(int i=0; i<3; i++){
     
    doc_snd["e"][i]["n"] = res[i];
    doc_snd["e"][i]["v"] = v;
  
    
  }
  
  /* 
  doc_snd["ID"] = "arduino" + type;
  for(int i=0; i<3; i++){
    doc_snd["EndPoints"][i] = res[i];
    doc_snd["AvailResurces"][i] = res[i]; 
  }
  
  doc_snd["EndPoints"][0] = res[0];
  doc_snd["EndPoints"][1] = res[1];
  doc_snd["EndPoints"][2] = res[2];
  
  doc_snd["AvailResurces"][0]["1"] = "Temperature";
  doc_snd["AvailResurces"][0]["2"] = "Temperature";
  doc_snd["AvailResurces"][0]["3"] = "Temperature";
 
  */
  
  
  String output;
  
  serializeJson(doc_snd, output);
  doc_snd.clear();
  Serial.println(output);
  return output;
}

void setup() {
  //:::::BOH
  Serial.begin(9600);
  while(!Serial);
  Serial.println("");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Bridge.begin();
  mqtt.begin("test.mosquitto.org", 1883);
  pinMode(PIR_PIN, INPUT);
  pinMode(NOISE_PIN, INPUT);
  mqtt.subscribe(my_base_topic + String("/led"), setLedValue); 

  pinMode(TEMP_PIN, INPUT);
  pinMode(INT_LED_PIN, OUTPUT);
  digitalWrite(INT_LED_PIN, LOW);
  Bridge.begin();
  digitalWrite(INT_LED_PIN, HIGH);

  attachInterrupt(digitalPinToInterrupt(NOISE_PIN), checkPresence2, RISING);

}


void loop() {
  
  float T0 = 298;
  float R0 = 100000;
  float B = 4275;
  float a = analogRead(TEMP_PIN);
  float R = ((1023.0 / a) - 1) * R0;
  float T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;

  String pubMsg = senMlEncode(pub, 1, "Pub");
  String subMsg = senMlEncode(sub, 1, "Sub");
  float val[] = {T, flag_sound, flag_pir};

  checkPir();
  checkSound(); 
  
  //Serial.println(postRequest(pubMsg));
  
  mqtt.monitor();
  mqtt.publish(String("iot/21/devices"), pubMsg);
  for(int i=0; i<3; i++){
    mqtt.publish(String("iot/21/") + pub[i], val[i]);
    Serial.println(pub[i]);
    Serial.println(val[i]);
  }
  mqtt.publish(String("iot/21/devices"), subMsg);

  
  delay(1000);

}
