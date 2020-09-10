#include <math.h>
#include <TimerOne.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

const int TEMP_PIN = A1;
const int FAN_PIN = 5;
const int LED_PIN = 6;
const int PIR_PIN = 8;
const int GLED_PIN = 10;
const int NOISE_PIN = 7;

volatile bool ORflag = false;
volatile int i=0;

//clap
volatile bool greenLedState = false;
volatile bool isClap = false;
volatile int clap_count = 0;
volatile int count_clap_noise = 0;

//noise
const unsigned long sound_interval = 60 * 1e03; 
const unsigned long timeout_sound = 3600 * 1e03; 
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

//ac && ht
volatile bool fan_on = false;
volatile bool modded = false;
volatile int acPerc, htPerc;
volatile float ac_min = 0, ac_max = 0, ht_min = 0, ht_max = 0;

//temperatura
volatile int a;
const float B = 4275;
const float R0 = 100000;
volatile float R;
const float T0 = 298;
volatile float T;

//pir
volatile bool presence_pir = false;
volatile unsigned long  prev_time_pir = 0;
const unsigned long timeout_pir = 1800 * 1e03; 
volatile unsigned long time_pir;
volatile int count_pir = 0;
volatile bool flag_pir = false;


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
  count_clap_noise++;
}

// ---------- CHECK TIME INTERVAL ---------------------------------

void checkPir(){

  checkPresence();

  time_pir = millis();
  
  if(time_pir - prev_time_pir >= timeout_pir || presence_pir == true ){  

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
  
  if(time_sound - prev_time_sound >= timeout_sound || presence_noise == true){ 

    if(time_sound - prev_time_sound >= time_sound && presence_noise == false){
       flag_sound=false;
    }

    prev_time_sound = time_sound;

    if(presence_noise == true){
      flag_sound = true;
    }
    
  }
}

//----------- CLAP LED ---------------------------------

void clap(){
  int clap_val_min, clap_val_max;
  
  if(fan_on == true){
    clap_val_min = 200;
    clap_val_max = 500;
  } else {
    clap_val_min = 100;
    clap_val_max = 300;
  }

  Serial.println(count_clap_noise);

  if(count_clap_noise>clap_val_min && count_clap_noise<clap_val_max){
    Serial.println("clap");
    isClap = true;
  } else {
    isClap = false;
  }
  count_clap_noise = 0;
}

void LEDclap(){
  clap();
  if(isClap == true){
    if(greenLedState == false){
      digitalWrite(GLED_PIN, HIGH);
      greenLedState = true;
     } else {
      digitalWrite(GLED_PIN, LOW);
      greenLedState = false;
     }
   }
    
  if(!ORflag && isClap == false){
    digitalWrite(GLED_PIN, LOW);
    greenLedState = false;
   }
}

//-------- HEATER AND CONDITIONER-----------------------------

float conditioning(float t, float minn, float maxx, float diff, int pin, bool ht){
  float stepd = (float) 255/diff;
  float yo, dc, pw;
  int flag;

  if(ht==true){
    flag= 255;
  } else {
    flag = 0;
  }
 
  if(t<minn){
    analogWrite(pin, flag - 0);
    return (ht ? 100 : 0);
  } else if(t>maxx){
    analogWrite(pin, 255 - flag);
    return (ht ? 0 : 100);
  } else {
    yo = (t-minn) * stepd;
    pw =  abs(flag-yo);
  
    analogWrite(pin, pw);
    dc = (float) pw/255;
  //  Serial.println(dc*100);
    return (dc * 100);
  }
}

void htANDac(){
  a = analogRead(TEMP_PIN);
  R = ((1023.0 / a) - 1) * R0;
  T  = (1 / ((log(R/R0) / B) + (1/T0))) - 273;
  
  if(Serial.available() > 0 ){
      ac_min = Serial.parseInt();
      ac_max = Serial.parseInt();
      ht_min = Serial.parseInt();
      ht_max = Serial.parseInt();
      Serial.read();
      if(ac_min!=0 && ac_max!=0 && ht_min!=0 && ht_max!=0){
        modded = true;
      }
      
  }
  if(ORflag == true && modded == false){
    ac_min = 26;
    ac_max = 28;
    ht_min = 17;
    ht_max = 20;
  }
  else if(ORflag == false && modded == false){
    ac_min = 25;
    ac_max = 30;
    ht_min = 15;
    ht_max = 20;
  }
      
    acPerc = conditioning(T, ac_min, ac_max, ac_max - ac_min, FAN_PIN, false);
    htPerc = conditioning(T, ht_min, ht_max, ht_max-ht_min, LED_PIN, true);

    
//    Serial.println(acPerc);
//    Serial.println(htPerc);
//    Serial.println(ac_min);
//    Serial.println(ac_max);
//    Serial.println(ht_min);
//    Serial.println(ht_max);
    


  if(acPerc > 0){
    fan_on = true;
  } else {
    fan_on = false;
  }
}
//-----------LCD PRINT ------------------

void printLCD(){
  
  if(i%2 == 0){
    printOnLCD1();
  } else {
    printOnLCD2();
  }

  i+=1;

  delay(2000);
}

void printOnLCD1(){
  lcd.home();
  lcd.clear();
  lcd.print("T:");
  lcd.print(T);
  lcd.print(" Pres:");
  lcd.print(ORflag);
  
  lcd.setCursor(0,1);
  lcd.print("AC:");
  lcd.print(acPerc);
  lcd.print("% ");
  lcd.print("HT:");
  lcd.print(htPerc);
  lcd.print("%");
  }

void printOnLCD2(){
  lcd.home();
  lcd.clear();
  lcd.print("AC m:");
  lcd.print(ac_min);
  lcd.print(" M:");
  lcd.print(ac_max);
  
  lcd.setCursor(0,1);
  lcd.print("HT m:");
  lcd.print(ht_min);
  lcd.print(" M:");
  lcd.print(ht_max);
}

//-----------------------------------

void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 2 Starting");
  
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(NOISE_PIN, INPUT);
  pinMode(GLED_PIN, OUTPUT);

  for(int i=0; i<10; i++){
    timez[i] = 0;
  }
  
  attachInterrupt(digitalPinToInterrupt(NOISE_PIN), checkPresence2, RISING);
  
  Serial.println("If you want customized temperature values insert: [AC_MIN] [AC_MAX] [HT_MIN] [HT_MAX]");
  
}

void loop() {

  checkPir();
  checkSound(); 

  ORflag = (flag_sound || flag_pir);
  
  /* AZIONE COMBINATA DI PIR E NOISE
  if(ORflag == true){
    Serial.println("There's someone in the room");
  } else {
    Serial.println("There's no one in the room");
  }
  */

  htANDac();

  LEDclap();

  printLCD();

}
