boolean boo = false;
void setup() {
  // put your setup code here, to run once:
 pinMode(2,INPUT);
 pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(2) == HIGH) boo = !boo;
  digitalWrite(13,boo);
  delay(100);
}
