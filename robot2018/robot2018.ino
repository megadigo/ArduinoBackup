#include <AFMotor.h>
#include <Servo.h>

// Contants
#define COLL_DIST 20 // sets distance at which the Obstacle avoiding Robot stops and reverses to 10cm
#define TURN_DIST COLL_DIST+10 // sets distance at which the Obstacle avoiding Robot looks away from object (not reverse) to 20cm (10+10)
#define MOTORS_CALIBRATION_OFFSET 0

int NCicles = 0;
// Eye
const int trigPin = 14;
const int echoPin = 15;

int maxAngle=0;
int maxDistance = -1;

int minAngle = 0;
int minDistance = 10000000;

int leftDistance = 0;
int frontDistance = 0;
int rightDistance = 0;

// Neck
Servo neck;

// DC Motors
AF_DCMotor rightMotor(1,MOTOR12_64KHZ); 
AF_DCMotor leftMotor(2,MOTOR12_64KHZ); 
String motorSet = "FORWARD";

void setup() {
  Serial.begin(9600); // Starts the serial communication
  // Neck Setup
  neck.attach(10);
  neck.write(90);
  delay(2000);

  //Motor Setup
  leftMotor.setSpeed(100); 
  rightMotor.setSpeed(100); 

  //Eye Setup
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
 
}

void loop() {
  NCicles++;
  //scan();
  //correctMove();
  Serial.print("NCicles ");
  Serial.print(NCicles);
  Serial.print("minAngle ");
  Serial.println(minAngle);

  if(NCicles <= 50) {
    moveForward();
  } else {
    neck.write(0);
    moveStop();
    
  }
  
  
}

void ClearSerial() {
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command
  
}
void correctMove() {
  //minAngle        angulo que leu a distancia minima
  //minDistance     distancia minima

  //leftDistance    minima distancia a esquerda
  //frontDistance   minima distancia em frente
  //rightDistance minima distancia a direita
  

  if(minAngle<80) {
    turnRight();
  } else if(minAngle>100) {
    turnLeft();
  } else {
    moveForward();
  }
  
 
}
void scan() {
  maxAngle=0;
  maxDistance = 0;

  minAngle = 90;
  minDistance = 5000;
  
  leftDistance = 5000;
  frontDistance = 5000;
  rightDistance = 5000;
  
  neck.write(144);
  delay(120);
  int angle=0;
  int distance=0;
  for(angle = 144; angle >= 36; angle -= 18){
    neck.write(angle);
    delay(90);
    distance = readEye();
    // Emergency stop
    
    // minDistance and minAngle
    if(distance<=minDistance) {
      minDistance=distance;
      minAngle = angle;
    }
    if(distance>=maxDistance) {
      maxDistance=distance;
      maxAngle = angle;
    }
    // Minn in quadrantes
    if(angle >= 90 && distance <= leftDistance) {
      leftDistance = distance;
    }    
    if(angle <= 90 && distance <= rightDistance) {
      rightDistance = distance;
    }
    if(angle == 90 && distance <= frontDistance) {
      frontDistance = distance;
    }
  }  
  
}

int readEye() {
  int duration = 0;
  int distance = 0;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034/2; 
  return distance;
}
  
// ------------------------------------
// motor commands
// -----------------------------------
void moveStop() {
  leftMotor.run(RELEASE);  // stop the motors.
  rightMotor.run(RELEASE);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void moveForward() {
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);      // turn it on going forward
  rightMotor.run(FORWARD);      // turn it on going forward
}
//-------------------------------------------------------------------------------------------------------------------------------------
void moveBackward() {
  motorSet = "BACKWARD";
  leftMotor.run(BACKWARD);      // turn it on going forward
  rightMotor.run(BACKWARD);     // turn it on going forward
}
//-------------------------------------------------------------------------------------------------------------------------------------
void turnRight() {
  motorSet = "RIGHT";
  leftMotor.run(FORWARD);      // turn motor 1 forward
  rightMotor.run(BACKWARD);     // turn motor 2 backward
}
//-------------------------------------------------------------------------------------------------------------------------------------
void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);     // turn motor 1 backward
  rightMotor.run(FORWARD);      // turn motor 2 forward
  delay(400); // run motors this way for 400ms
}
//-------------------------------------------------------------------------------------------------------------------------------------
void lookRight() {
  rightMotor.run(BACKWARD);  // looking right? set right motor backwards for 400ms
  delay(400);
  rightMotor.run(FORWARD);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void lookLeft() {
  leftMotor.run(BACKWARD);  // looking left? set left motor backwards for 400ms
  delay(400);
  leftMotor.run(FORWARD);
}
//------------------------------------

