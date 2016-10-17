#include <SoftwareSerial.h>
SoftwareSerial blue(2,3);

char NOME[21] = "BLUE";
char BPS = '4'; //1=1200, 2=2400, 3=4800, 4=9600, 5=19200, 6=38400, 7=57600, 8=115200
char PASS[5] = "1234";

void setup() {
  // put your setup code here, to run once:
  blue.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delay(4000);
  digitalWrite(13,LOW);

  blue.print("AT");
  delay(1000);

  blue.print("AT+NAME");
  blue.print(NOME);
  delay(1000);

  blue.print("AT+BAUD");
  blue.print(BPS);
  delay(1000);

  blue.print("AT+PIN");
  blue.print(PASS);
  delay(1000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13,!digitalRead(13));
  delay(2000);
}
