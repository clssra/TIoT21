
volatile float current_speed = 0;
float duty_cycle = 0;

const int FAN_PIN = 5;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Lab 1.4 Starting");
  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, (int) current_speed);
}

void loop() {
  if(Serial.available() > 0){
    int inByte = Serial.read();

    if(inByte == '+' && (current_speed +15) <= 255){
      current_speed += 15;
      analogWrite(FAN_PIN, (int) current_speed);
      Serial.print("Current Speed: ");
      Serial.println(current_speed);
      
    } else if(inByte == '-' && (current_speed-15) >= 0){
      current_speed -= 15;
      analogWrite(FAN_PIN, (int) current_speed);
      Serial.print("Current Speed: ");
      Serial.println(current_speed);
      
    } else {
      if(current_speed + 15 > 255){
        Serial.println("Already at max speed");
      } else if (current_speed - 15 < 0){
        Serial.println("Already at min speed");
      } else {
        Serial.println("Invalid Command");
      }
    }
  }

}
