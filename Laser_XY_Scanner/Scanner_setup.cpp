#include "Scanner_setup.h"

byte hMinVal;
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepperX(HALFSTEP, s1_in1, s1_in3, s1_in2, s1_in4);
AccelStepper stepperY(HALFSTEP, s2_in5, s2_in7, s2_in6, s2_in8);

void stepperInit(float speed, float maxSpeed, float acceleration){
  pinMode(PIN_X_MIN, INPUT);           // set pin to input
  pinMode(PIN_Y_MIN, INPUT);
  digitalWrite(PIN_X_MIN, HIGH);       // turn on pullup resistors
  digitalWrite(PIN_Y_MIN, HIGH);


  //Serial.println("Homing X");
  stepperX.setMaxSpeed(1000);     // slow speed for calibrating
  //stepperX.moveTo(MAX_STEPPER);   // try to move to max
  stepperX.setAcceleration(1000);
  xStepperHome();                 // runs routine to home X motor
  //Serial.println("Homing Y");
  stepperY.setMaxSpeed(1000);     // slow speed for calibrating
  //stepperY.moveTo(MAX_STEPPER);   // try to move to max
  stepperY.setAcceleration(1000);
  yStepperHome();                 // runs routine to home Y motor
  //Serial.println("Calibration complete");

  //Set the initial speed (read the AccelStepper docs on what "speed" means)
  stepperX.setSpeed(speed);
  stepperX.setMaxSpeed(maxSpeed);
  stepperX.setAcceleration(acceleration);
  stepperY.setSpeed(speed);
  stepperY.setMaxSpeed(maxSpeed);
  stepperY.setAcceleration(acceleration);
}

void xStepperHome(){
  hMinVal = digitalRead(PIN_X_MIN);
  while (hMinVal == LOW)
  {
    //backwards slowly till it hits the switch and stops
    stepperX.moveTo(MAX_STEPPER);
    stepperX.run();
    hMinVal = digitalRead(PIN_X_MIN);
  }
  stepperX.setCurrentPosition(0); //set current motor position to zero
  stepperX.moveTo(STEPPER_HOME_X);
  while (stepperX.distanceToGo() != 0){
    stepperX.run();
  }
  stepperX.setCurrentPosition(0); //set current motor position to zero
}

void yStepperHome(){
  hMinVal = digitalRead(PIN_Y_MIN);
  while (hMinVal == LOW)
  {
    //backwards slowly till it hits the switch and stops
    stepperY.moveTo(MAX_STEPPER);
    stepperY.run();
    hMinVal = digitalRead(PIN_Y_MIN);
  }
  stepperY.setCurrentPosition(0); //set current motor position to zero
  stepperY.moveTo(STEPPER_HOME_y);
  while (stepperY.distanceToGo() != 0){
    stepperY.run();
  }
  stepperY.setCurrentPosition(0); //set current motor position to zero
}
