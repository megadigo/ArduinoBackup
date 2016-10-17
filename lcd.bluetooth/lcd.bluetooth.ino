#include <SoftwareSerial.h>
#define TXPIN 2
#define RXPIN 3
SoftwareSerial BlueSerial(RXPIN,TXPIN);

void setup() {
  // put your setup code here, to run once:
  BlueSerial.begin(115200);
  Serial.begin(115200);
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);
  pinMode(13,OUTPUT);
  Serial.println("Serial Started");
  
  digitalWrite(13,HIGH);
  delay(4000);
  digitalWrite(13,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13,!digitalRead(13));
  char inChar = BlueSerial.read();
  if(lenght(inChar) != 0)
  {
  Serial.println(inChar)
  };
}
