void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Valor lido=");
  Serial.print(analogRead(A0)*5.00/1023*100);
  Serial.println("*C");
  delay(1000);
}
