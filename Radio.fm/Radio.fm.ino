
#include <Wire.h>
#include <TEA5767Radio.h>

TEA5767Radio radio = TEA5767Radio();


void setup() {
  // put your setup code here, to run once:
  Wire.begin(); 
  radio.setFrequency(100.4); // pick your own frequency

}

void loop() {
  // put your main code here, to run repeatedly:

}
