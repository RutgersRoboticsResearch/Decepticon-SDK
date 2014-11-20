#include <NewPing.h>
#include <Servo.h>
#include <string.h>
#include <stdlib.h>

#define LEFTSIDE_NEG   9
#define LEFTSIDE_POS   10
#define RIGHTSIDE_NEG  5
#define RIGHTSIDE_POS  6
#define CLAW_PIN       3
#define TRIGGER_PIN    12
#define ECHO_PIN       11
#define MAX_DISTANCE   200
#define INTERVAL       50

char buf[256];
Servo claw;
int clawSpeed;
int leftSpeed;
int rightSpeed;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned long oldTime;

// Limit the Servo between 0 and 180
int limitServo(int x) {
  return (x > 180) ? 180 : ((x < 0) ? 0 : x);
}

// If the value is between 0-255, then return as is
// If the value is between 256-510, then subtract 255, and return that value as negative
// Else return 0
int limitPWM(int x) {
  if (x > 255)
    x = -(x - 255);
  if (x < -255)
    x = 0;
  return x;
}

// Set the speeds based on what was read from Serial Buffer
void setSpeeds() {
  char temp[4];
  if (strlen(buf) != 9) {
    //Serial.print("Invalid message: ");
    //Serial.println(buf);
    return;
  }
  for (int i = 0; i < 3; i++) {
    memcpy(temp, &buf[i * 3], 3 * sizeof(char));
    temp[3] = '\0';
    switch (i) {
      case 0:
        clawSpeed = limitServo(atoi(temp));
        break;
      case 1:
        leftSpeed = limitPWM(atoi(temp));
        break;
      case 2:
        rightSpeed = limitPWM(atoi(temp));
        break;
    }
  }
  //printSpeeds();
}

// Print out the Speeds to the Serial Console
void printSpeeds() {
  Serial.print("Claw now: ");
  Serial.println(clawSpeed);
  Serial.print("Leftside now: ");
  Serial.println(leftSpeed);
  Serial.print("Rightside now: ");
  Serial.println(rightSpeed);
}

void setup() {
  // Begin Serial
  Serial.begin(115200);

  // variable inits
  clawSpeed = 90;
  leftSpeed = 0;
  rightSpeed = 0;
  claw.attach(CLAW_PIN);
  pinMode(LEFTSIDE_NEG, OUTPUT);
  pinMode(LEFTSIDE_POS, OUTPUT);
  pinMode(RIGHTSIDE_NEG, OUTPUT);
  pinMode(RIGHTSIDE_POS, OUTPUT);
  oldTime = millis();  
  // READY!
  //Serial.println("Good to go!");
  //printSpeeds();
}

void loop() {
  // Read from Serial
  if (Serial.available() > 0) {
    int n;
    n = Serial.readBytesUntil('\n', buf, 255);
    setSpeeds();
  }

  // Write the speeds
  claw.write(clawSpeed);
  if (leftSpeed < 0) {
    analogWrite(LEFTSIDE_POS, 0);
    analogWrite(LEFTSIDE_NEG, abs(leftSpeed));
  } else {
    analogWrite(LEFTSIDE_NEG, 0);
    analogWrite(LEFTSIDE_POS, abs(leftSpeed));
  }
  if (rightSpeed < 0) {
    analogWrite(RIGHTSIDE_POS, 0);
    analogWrite(RIGHTSIDE_NEG, abs(rightSpeed));
  } else {
    analogWrite(RIGHTSIDE_NEG, 0);
    analogWrite(RIGHTSIDE_POS, abs(rightSpeed));
  }
  
  // print data back to serial
  if (millis() - oldTime >= INTERVAL) {
    oldTime = millis();
    unsigned int distance_cm = sonar.ping_cm();
    Serial.println(distance_cm);
  } 
}
