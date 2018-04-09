// Programa : Arduino Motor Shield - Controle motores DC
// Autor : Arduino e Cia
 
// carrega a biblioteca AFMotor
#include <AFMotor.h>

//Sensores
// defines pins numbers

const int trigPinr = 14;
const int echoPinr = 15;

const int trigPinl = 16;
const int echoPinl = 17;

// defines variables
long durationl;
long distancel;

long durationr;
long distancer;

int status;

// Define o motor1 ligado a conexao 1
AF_DCMotor motorr(3,MOTOR12_64KHZ); 
// Define o motor2 ligado a conexao 4
AF_DCMotor motorl(4,MOTOR12_64KHZ); 
 
void setup()
{
  
  // Set sensor pins
  pinMode(trigPinl, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPinl, INPUT); // Sets the echoPin as an Input
  pinMode(trigPinr, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPinr, INPUT); // Sets the echoPin as an Input
  
  // Define a velocidade maxima para os motores 1 e 2
  motorr.setSpeed(255); 
  motorl.setSpeed(255);
  // Serial
  Serial.begin(9600); // Starts the serial communication
}
 
void loop()
{
  //////////////////////
  // Collect Sensores //
  //////////////////////

  // LEFT EYE
  // Clears the trigPinl
  digitalWrite(trigPinl, LOW);
  delayMicroseconds(2);
  // Sets the trigPinl on HIGH state for 10 micro seconds
  digitalWrite(trigPinl, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinl, LOW);
  // Reads the echoPinl, returns the sound wave travel time in microseconds
  durationl = pulseIn(echoPinl, HIGH);
  // Calculating the distance
  distancel= durationl*0.034/2;
  //Serial.print("LEFT=");
  //Serial.println(distancel);

  // RIGHT EYE
  // Clears the trigPinr
  digitalWrite(trigPinr, LOW);
  delayMicroseconds(2);
  // Sets the trigPinr on HIGH state for 10 micro seconds
  digitalWrite(trigPinr, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinr, LOW);
  // Reads the echoPinr, returns the sound wave travel time in microseconds
  durationr = pulseIn(echoPinr, HIGH);
  // Calculating the distance
  distancer= durationr*0.034/2;
  //Serial.print("RIGHT=");
  //Serial.println(distancer);

    if(distancel>30 && distancer>30){
      status=0;
    } else if(distancel==distancer){
      status=2;
    } else if(distancel>=distancer ) {
      status=1;
    } else if(distancer>=distancel) {
      status=3;
    }

  if(status==0){ // stop
   //motorr.run(RELEASE);
   //motorl.run(RELEASE);
  }
  if(status==1){ // left
   motorr.run(FORWARD);
   motorl.run(BACKWARDS);
   delay(100);
  }
  if(status==2){ // forward
   motorr.run(FORWARD);
   motorl.run(FORWARD);
  }
  if(status==3){ // right
   motorr.run(BACKWARDS);
   motorl.run(FORWARD);
  }
}

