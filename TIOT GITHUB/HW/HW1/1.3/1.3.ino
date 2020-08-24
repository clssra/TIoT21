#include <TimerOne.h>

const int LED_PIN = 12;
const int PIR_PIN = 7;

volatile int count = 0;


void checkPresence(){  
  count++;
  digitalWrite(LED_PIN, HIGH);
   
  delay(500);
  digitalWrite(LED_PIN, LOW); 
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 1.3 Starting");
  
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), checkPresence, FALLING);
  
}

void loop() {
    Serial.print("Total people count: ");
    Serial.println(count);
    
    delay(30000);
}
