// ARDUINO NANO 5V
// HC-60        RX=3 TX=2 GND E VND5V
// LCD          16x2 SDA=A1 SCL=A2 GND e VND5V
#include <SoftwareSerial.h>
#define RXPIN 2
#define TXPIN 3
SoftwareSerial BlueSerial(RXPIN, TXPIN);
void setup() {
  // put your setup code here, to run once:
  
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);
  // Initilize serials
  Serial.begin(9600);
  BlueSerial.begin(9600);
  Serial.println("Serial start!");
  BlueSerial.println("BlueSerial start!");
}

void loop() {
  // put your main code here, to run repeatedly:
   if(BlueSerial.available() > 0)
    {
        // Read off all bytes
        char val = BlueSerial.read();
        Serial.print(val);
        delay(100);  
    }
}
