void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Valor lido=");
  Serial.print(analogRead(A0));
  Serial.println("bit");
  delay(1000);
}
