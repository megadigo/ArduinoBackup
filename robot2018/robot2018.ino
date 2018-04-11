#include <AFMotor.h>
#include <Servo.h>

// Contants
#define COLL_DIST 20 // sets distance at which the Obstacle avoiding Robot stops and reverses to 10cm
#define TURN_DIST COLL_DIST+10 // sets distance at which the Obstacle avoiding Robot looks away from object (not reverse) to 20cm (10+10)
#define MOTORS_CALIBRATION_OFFSET 0
#define MOTORS_MAXSPEED 80
#define EYE_TRIGGER_PIN 14
#define EYE_ECHO_PIN 15
#define NECK_PIN 10
#define RIGHT_MOTOR_PIN 1
#define LEFT_MOTOR_PIN 2

// Loop Vars
int NCicles = 0;

// Eye Vars
int distance = 0;
int maxAngle=0;
int maxDistance = -1;
int minAngle = 0;
int minDistance = 10000000;
int angle = 0;
int leftDistance = 0;
int frontDistance = 0;
int rightDistance = 0;

// Neck
Servo neck;

// DC Motors
AF_DCMotor rightMotor(RIGHT_MOTOR_PIN,MOTOR12_64KHZ); 
AF_DCMotor leftMotor(LEFT_MOTOR_PIN,MOTOR12_64KHZ); 
String motorSet = "FORWARD";

void setup() {
  // Serial Setup
  Serial.begin(9600); // Starts the serial communication
  
  // Neck Setup
  neck.attach(NECK_PIN);
  neck.write(90);
  delay(2000);

  //Motor Setup
  leftMotor.setSpeed(MOTORS_MAXSPEED - (MOTORS_CALIBRATION_OFFSET / 2)); 
  rightMotor.setSpeed(MOTORS_MAXSPEED + (MOTORS_CALIBRATION_OFFSET / 2)); 

  //Eye Setup
  pinMode(EYE_TRIGGER_PIN, OUTPUT); // Sets the EYE_TRIGGER_PIN as an Output
  pinMode(EYE_ECHO_PIN, INPUT);  // Sets the EYE_ECHO_PIN as an Input
  
  maxAngle=0;
  maxDistance = 0;

  minAngle = 90;
  minDistance = 5000;
  
  leftDistance = 5000;
  frontDistance = 5000;
  rightDistance = 5000;

  // Neck Setup
  int angle=144;
  neck.write(angle);
  int distance=0;
  
}

void loop() {
  
  NCicles++; 

  // Scan 
  neck.write(angle);
  delay(90);
  readEye;
  // Calculate distances
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
    
  if(angle<=36) {
    // End neck rotation reset distance and angle
    angle=144;
    maxAngle=0;
    maxDistance = 0;
    minAngle = 90;
    minDistance = 5000;
    // Correct the movement
      correctMove();
    //
  } else {
    angle -=18;
  }
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

void readEye() {
  int duration = 0;
  // Clears the EYE_TRIGGER_PIN
  digitalWrite(EYE_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  // Sets the EYE_TRIGGER_PIN on HIGH state for 10 micro seconds
  digitalWrite(EYE_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(EYE_TRIGGER_PIN, LOW);
  // Reads the EYE_ECHO_PIN, returns the sound wave travel time in microseconds
  duration = pulseIn(EYE_ECHO_PIN, HIGH);
  // Calculating the distance
  distance = duration * 0.034/2; 
}

void moveStop() {
  leftMotor.run(RELEASE);  // stop the motors.
  rightMotor.run(RELEASE);
}

void moveForward() {
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);      // turn it on going forward
  rightMotor.run(FORWARD);      // turn it on going forward
}

void moveBackward() {
  motorSet = "BACKWARD";
  leftMotor.run(BACKWARD);      // turn it on going forward
  rightMotor.run(BACKWARD);     // turn it on going forward
}

void turnRight() {
  motorSet = "RIGHT";
  leftMotor.run(FORWARD);      // turn motor 1 forward
  rightMotor.run(BACKWARD);     // turn motor 2 backward
}

void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);     // turn motor 1 backward
  rightMotor.run(FORWARD);      // turn motor 2 forward
  delay(400); // run motors this way for 400ms
}

void lookRight() {
  rightMotor.run(BACKWARD);  // looking right? set right motor backwards for 400ms
  delay(400);
  rightMotor.run(FORWARD);
}

void lookLeft() {
  leftMotor.run(BACKWARD);  // looking left? set left motor backwards for 400ms
  delay(400);
  leftMotor.run(FORWARD);
}

