#ifndef __SCANNER_SETUP_H
#define __SCANNER_SETUP_H

#include <Arduino.h>
#include <AccelStepper.h>
#include "pointer_with_backlash.h"

#define HALFSTEP 8
#define FULLSTEP 4

// AccelStepper pins
#define s1_in1 2
#define s1_in2 3
#define s1_in3 4
#define s1_in4 5

#define s2_in5 6
#define s2_in6 7
#define s2_in7 8
#define s2_in8 9

#define PIN_X_MIN 10        // define Min/home inputs
#define PIN_Y_MIN 11

#define MAX_STEPPER 4096
#define STEPPER_HOME_X -827
#define STEPPER_HOME_y -725


//extern byte hMinVal;               // temporary Min input variable
extern AccelStepper stepperX;
extern AccelStepper stepperY;
extern PointerWithBacklash backlashX;
extern PointerWithBacklash backlashY;

void stepperInit(float speed, float maxSpeed, float acceleration);

void xStepperHome(); //this routine should run the motor

void yStepperHome(); //this routine should run the motor

#endif
