void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4,HIGH);
  delay(4000);
  digitalWrite(4,LOW);
  digitalWrite(3,HIGH);
  delay(2000);
  digitalWrite(3,LOW);
  digitalWrite(2,HIGH);
  delay(4000);
  digitalWrite(2,LOW);
}
