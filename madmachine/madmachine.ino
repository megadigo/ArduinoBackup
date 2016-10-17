#include<Servo.h>
Servo sm;
void setup() {
  // put your setup code here, to run once:
pinMode(2,INPUT);
pinMode(3,INPUT);
pinMode(8,OUTPUT);
pinMode(9,OUTPUT);
Serial.begin(9600);
sm.attach(3);
}
int estado = 0; //"idle";
float tempa = 0;
float lux = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(2)==HIGH) {
    digitalWrite(8,LOW);
    digitalWrite(9,LOW);
    
    if (estado == 2) {
      estado = 1;
    } else {
      estado = 2;
    }
    delay(200);
  }
  switch (estado){
    case 1: //temperatura
      digitalWrite(9,HIGH);
      //le a temperatura e activa o servo
      tempa = analogRead(A0)*5.00/1023*100;
      Serial.print("Valor lido=");
      Serial.print(tempa);
      Serial.println("*C");
      sm.write(180-tempa*180.00/100);
      delay(500);
      break;    
    case 2: //luz
      digitalWrite(8,HIGH);
      //le a luz e activa o servo
      lux = 180.00-analogRead(A1)*180.00/1024.00;
      Serial.print("Valor lido=");
      Serial.print(lux);
      Serial.println(" bit");
      sm.write(lux);
      delay(500);
      break;
    default:
      estado = 1;
      // calibrar o servo e o grafico
      //sm.write(0);
      //sm.write(90);
      //sm.write(180);
  }
}
