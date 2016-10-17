boolean boo = false;
void setup() {
  // put your setup code here, to run once:
 pinMode(2,INPUT);
 pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(2) == HIGH) {
    if (boo == true) {
      digitalWrite(13,HIGH);
      boo = false;
    }  else {
      digitalWrite(13,LOW);
      boo = true;
    }
  }
}
