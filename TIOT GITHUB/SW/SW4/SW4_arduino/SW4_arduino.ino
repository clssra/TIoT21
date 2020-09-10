#include <Bridge.h>
#include <Process.h>
#include <MQTTclient.h>

#define LEDPIN 10
#define NOISEPIN 6
#define MOVEPIN 7

bool movement=false;
int movementtime=0;
bool noise=false;
int noisetime=0;
int noisec=0;

void movementFunct(){
  Serial.println("Movement detected");
  movement=true;
  movementtime=millis();
  }

void noiseFunct(){
  if(noisec>20){
    Serial.println("Noise detected");
    noise=true;
    noisetime=millis();
    noisec=0;
    }
  }

void noiseCount(){noisec++;}

void setup(){
    pinMode(NOISEPIN,INPUT);
    pinMode(MOVEPIN, INPUT);
    pinMode(LEDPIN,OUTPUT);
    
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Lab 4SW Starting");
    
    attachInterrupt(digitalPinToInterrupt(NOISEPIN), noisecount, FALLING);
    
    Bridge.begin();

    String data = "{\"bn\": \"Arduino\", \"e\": [{\"t\":\"0\",\"n\": \"alarm\",\"v\": \"0\", \"u\":\"null\"}]}";
    int exit_val = my_curl(data);
    Serial.println(exit_val);
    
    mqtt.begin("test.mosquitto.org",1883);
}

void loop() {
  //if che simula un interrupt per il sensore di rumore
  if(digitalRead(MOVEPIN)==HIGH){
    movememntFunct();}
    noiseFunct();
  //se solo uno dei due sensori si è attivato in un minuto si annulla la flag
  if(millis()-noisetime>60000){
    noise=false;}
  if(millis()-movementtime>60000){
    movement=false;}
    
  //se entrambi i sensori si sono attivati viene pubblicato attraverso mqtt un solo allarme e si annullano le flag
  if(movement && noise){
    digitalWrite(LEDPIN, HIGH);
    mqtt.publish("iot/21/alarm","{\"bn\": \"Arduino\", \"e\": [{\"t\":\"0\",\"n\": \"alarm\",\"v\": \"1\", \"u\":\"bool\"}]}");
    movement=false;
    noise=false;
    }
  
  if(!movement && !noise){
    digitalWrite(LEDPIN, LOW);}
  }

int my_curl(String data){
    Process p;
    p.begin("curl");
    p.addParameter("-H");
    p.addParameter("Content-Type: application/json");
    p.addParameter("-X");
    p.addParameter("POST");
    p.addParameter("-d");
    p.addParameter(data);
    p.addParameter("http://172.20.10.2:8080/devices");
    p.run();
    
    return(p.exitValue());
}
