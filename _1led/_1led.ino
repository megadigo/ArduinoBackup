void setup() {
  // put your setup code here, to run once:

}
boolean boo = false;
void loop() {
  // put your main code here, to run repeatedly:
  pinMode(13,OUTPUT);
  boo = !boo;
  if (boo==true) {
    digitalWrite(13,HIGH);
  } else {
    digitalWrite(13,LOW);
  }
  delay(100);
}
