void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Valor lido=");
  Serial.print(analogRead(A1)*5.00/1023);
  Serial.println( "V");
  delay(1000);
}
