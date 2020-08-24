#include <math.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

const int TEMP_PIN = A1;
const float B = 4275;
const float R0 = 100000;
const float T0 = 298;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 1.6 Starting");

  lcd.begin(16,2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.print("Temperature: ");
  
}

void loop() {
  int a = analogRead(TEMP_PIN);
  float R = ((1023.0/a)-1)*R0;
  float T  = 1/((log(R/R0)/B)+(1/T0));
  
  Serial.print("temperature = ");
  Serial.println(T-273);

  lcd.setCursor(12, 0);
  lcd.print(T-273);

   delay(1000);
}
