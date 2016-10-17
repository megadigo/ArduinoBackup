#include<Servo.h>
Servo sm;
void setup() {
  // put your setup code here, to run once:
  sm.attach(3);
  pinMode(13,OUTPUT);
}
// servo faz 0º a 180º
void loop() {
  // put your main code here, to run repeatedly:
  sm.write(0);
  digitalWrite(13,HIGH);
  delay(1000);
  sm.write(90); //graus;
  digitalWrite(13,LOW);
  delay(1000);
}
