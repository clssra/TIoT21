#include <TimerOne.h>

const int RLED_PIN = 12;
const int GLED_PIN = 11;

const float G_HALF_PERIOD = 3.5;
const float R_HALF_PERIOD = 1.5;

int redLedState = LOW;
int greenLedState = LOW;

void blinkGreen() {
  greenLedState = !greenLedState;
  digitalWrite(GLED_PIN, greenLedState);
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 1.2 Starting");
  
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);

  Timer1.initialize(G_HALF_PERIOD * 1e06);
  Timer1.attachInterrupt(blinkGreen);

}

void serialPrintStatus(){
  if(Serial.available() > 0){
    int inByte = Serial.read();

    if(inByte == 'R'){
      Serial.print("Red Status: ");
      Serial.println(redLedState);
    } else if(inByte == 'G'){
      Serial.print("Green Status: ");
      Serial.println(greenLedState);
    } else {
      Serial.println("Invalid Command");
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  redLedState = !redLedState;
  digitalWrite(RLED_PIN, redLedState);
  delay(R_HALF_PERIOD * 1e03);
  serialPrintStatus();
}
