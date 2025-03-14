/*
  Laser XY Scanner
  Low Cost DIY stepper motor Laser XY Scanner, Cutter, or Engraver

  Laser XY Scanner is an open source stepper motor based scanner. 
  The components used are low cost and very accessible. This type 
  of laser scanner is not extremely fast but it can still be used 
  in a variety of applications. Draw, cut and or point with a 
  laser with X and Y control.

  The AccelStepper library is required for this sketch to run.
  Download and documentation here: http://aka.ms/AccelStepper
  AccelStepper Author: Mike McCauley (mikem@airspayce.com)

  GitHub Project: https://github.com/ThingEngineer/Laser-XY-Scanner
  Thingiverse files for CHICK: https://www.thingiverse.com/thing:2710048
  Instructable: https://www.instructables.com/id/Low-Cost-DIY-Stepper-Motor-Laser-XY-Scanner-Cutter/

  Functions:
  - Draw, cut and or point with a laser with X and Y control.

 The circuit:
  * Arduino Nano
    * Monitors small lever switches for X and Y home detection (minimum travel)
    * Controls ULN2003APG high−current darlington drivers
      * Drives 28BYJ-48 5V Stepper Motors
  * 12V TTL 1W/1.6W/2W/445nm/450nm Laser Diode LD Driver
  * Drives 2W 445nm M140 Blue Laser Diode w/ AixiZ aluminum mount and heat sink for 12mm modules

 Created 3 April 2017
 Modified 12 December 2017
 Author Josh Campbell

 v1.0 - 12/12/2017
 v1.1 - 12/13/2017  -Documentation changes only

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
 */

//#define MESSAGES
#define PACKET_SIZE 5

#include <AccelStepper.h>
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

#define MIN_X -500
#define MAX_X 500
#define MIN_Y -500
#define MAX_Y 500

#define HEADER 0x5e   //"^" dec 94
#define ACK 0x06
#define NACK 0x15

const int xMin = 10;        // define Min/home inputs
const int yMin = 11;
byte hMinVal;               // temporary Min input variable

const int LASER = 12;       // define Laser switch output

//Keeps track of the current direction relative to the face of the motor. 
//Clockwise (true) or Counterclockwise(false)
bool xClockwise = true;     // default to clockwise
bool yClockwise = true;     // default to clockwise

int TargetArrSquare[] = {0, 1, 0, 1}; // 0 = x, 1 = y
int TargetPositionArrSquare[] = {150, 150, -150, -150};

int TargetPositionArrTriangleX[] = {100, 100,   -150};
int TargetPositionArrTriangleY[] = {-150,   150, -150};

int slowDrawProgressX = 0;
int slowDrawProgressY = 0;

int moveCount = 0;          // loop used for drawing

char headerCounter = 0;

enum ErrorState {
  NO_ERROR = 0,
  OUT_OF_BOUNDS = 1 << 0,
  INVALID_HEADER = 1 << 1,
};

ErrorState stepperError;

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepperX(HALFSTEP, s1_in1, s1_in3, s1_in2, s1_in4);
AccelStepper stepperY(HALFSTEP, s2_in5, s2_in7, s2_in6, s2_in8);

inline ErrorState operator|(ErrorState a, ErrorState b) {
  return static_cast<ErrorState>(static_cast<int>(a) | static_cast<int>(b));
}
inline ErrorState& operator|=(ErrorState& a, ErrorState b) {
  a = static_cast<ErrorState>(static_cast<int>(a) | static_cast<int>(b));
  return a;
}

void setup() {

  Serial.begin(115200);
  pinMode(LASER, OUTPUT);         // set pin to output
  digitalWrite(LASER, LOW);       // turn off the laser (for the calicration)

  // Setup home switch inputs (minimum travel)
  pinMode(xMin, INPUT);           // set pin to input
  pinMode(yMin, INPUT);
  digitalWrite(xMin, HIGH);       // turn on pullup resistors
  digitalWrite(yMin, HIGH);

  //Serial.println("Homing X");
  stepperX.setMaxSpeed(1000);     // slow speed for calibrating
  stepperX.moveTo(4096);          // try to move to max
  stepperX.setAcceleration(1000);
  xStepperHome();                 // runs routine to home X motor
  //Serial.println("Homing Y");
  stepperY.setMaxSpeed(1000);     // slow speed for calibrating
  stepperY.moveTo(4096);          // try to move to max
  stepperY.setAcceleration(1000);
  yStepperHome();                 // runs routine to home Y motor
  //Serial.println("Calibration complete");

  //Set the initial speed (read the AccelStepper docs on what "speed" means)
  stepperX.setSpeed(1000.0);
  stepperX.setMaxSpeed(1000.0);
  stepperX.setAcceleration(20000.0);
  stepperY.setSpeed(1000.0);
  stepperY.setMaxSpeed(1000.0);
  stepperY.setAcceleration(20000.0);

  //while(!Serial){}
  stepperError = NO_ERROR;
  digitalWrite(LASER, HIGH);       // turn on the laser
}

void loop() {

  // Uncomment only one drawing function at a time, then experiment to make your own!

  //calibrate();      // Helper method to manually steer motors with the help of serial. 

  serialControl();

  //drawSquare();     // Draw a square, uses values in TargetArr and TargetPositionArr

  //slowTriangle();

  //xyScan();       // Example XY Scan (change either x or y speed to around 30, one slow one fast)

  //If the stepper still needs to move (distanceToGo() != 0) then continue to advance (step) the motor
  stepperX.run();
  stepperY.run();
}
/*
void calibrate() {
  if(stepperX.distanceToGo() == 0 and stepperY.distanceToGo() == 0){
    char messageSet[50], messageTarget[50];
    int targetX, targetY;
    sprintf(messageSet, "Current x: %ld; Current y: %ld", stepperX.currentPosition(), stepperY.currentPosition());
    Serial.println(messageSet);
    while (Serial.available() == 0)
      ;
    targetX = Serial.parseInt();
    targetY = Serial.parseInt();
    sprintf(messageTarget, "Target x: %d; Target y: %d", targetX, targetY);
    Serial.println(messageTarget);
    stepperX.moveTo(targetX);
    stepperY.moveTo(targetY);
  }
}
*/

void serialControl(){
  if (stepperX.distanceToGo() == 0 and stepperY.distanceToGo() == 0) {
    //stepperX.currentPosition(), stepperY.currentPosition();
    if (Serial.available() >= PACKET_SIZE) {
      if (stepperError == NO_ERROR) Serial.write(ACK);
      else Serial.write(NACK);
      headerCounter = 0;
      stepperError = NO_ERROR;
#ifdef MESSAGES
      char message[90];
#endif
      for (int i = Serial.available(); i>0; i = Serial.available()){
        /*
        int16_t target[2];
        Serial.readBytes(NULL, 1);
        Serial.readBytes((char*)&target, 4);
        stepperX.moveTo(target[0]);
        stepperY.moveTo(target[1]);
        snprintf(message, sizeof(message), "X: %lX, Y: %lX", target[0], target[1]);
        //snprintf(message, sizeof(message), "%04X %04X %04X %04X %04X %04X", target[0], target[1], target[2], target[3], target[4], target[5]);
        //snprintf(message, sizeof(message), "%08lX %08lX %08lX %08lX %08lX %08lX", target[0], target[1], target[2], target[3], target[4], target[5]);
        //snprintf(message, sizeof(message), "%lX %lX", target[0], target[1]);
        //*/

        char head;
        int16_t targetX, targetY;
        Serial.readBytes((char*)&head, 1);

        if (head != HEADER){
          // if PACKET_SIZE ammount of bytes have been read, the packet could've ben misread
          headerCounter++;
          if (headerCounter >= PACKET_SIZE){
            stepperError |= INVALID_HEADER;
            break;
          }
          // if trash data were at the beginning, ignore it and try again
          continue;
        }
        Serial.readBytes((char*)&targetX, 2);
        Serial.readBytes((char*)&targetY, 2);
        if (MIN_X > targetX || targetX > MAX_X || MIN_Y > targetY || targetY > MAX_Y) {
          stepperError |= OUT_OF_BOUNDS;
          // Probably a packet was misread. Negative-acknowledge master
          break;
        }

        if (stepperError == NO_ERROR) {
          stepperX.moveTo(targetX);
          stepperY.moveTo(targetY);
          // Acknowledge master
          break;
        }
      }
#ifdef MESSAGES
      snprintf(message, sizeof(message), "X: %d, Y: %d", targetX, targetY);
      //snprintf(message, sizeof(message), "A: %lX, B: %lX", targetX, targetY);
      //*/
      Serial.println(message);
#endif
    }
  }
}
/*
void drawSquare(){
  // Draw a square  
  if(stepperX.distanceToGo() == 0 and stepperY.distanceToGo() == 0){
    if(moveCount == 3){
      moveCount = 0;
    } else {
      moveCount = moveCount + 1;
    }

    if (TargetArrSquare[moveCount] == 0) {
      stepperX.moveTo(TargetPositionArrSquare[moveCount]);
    }
    else {
      stepperY.moveTo(TargetPositionArrSquare[moveCount]);
    }
  }
}

void slowTriangle(){
  if(stepperX.distanceToGo() == 0 and stepperY.distanceToGo() == 0){
    if (slowDrawProgressX == TargetPositionArrTriangleX[moveCount] and slowDrawProgressY == TargetPositionArrTriangleY[moveCount]) {
      if(moveCount == 2){
        moveCount = 0;
      } else {
        moveCount = moveCount + 1;
      }

      //slowDrawProgressX = TargetPositionArrTriangleX[moveCount];
      //slowDrawProgressY = TargetPositionArrTriangleY[moveCount];
    }
    
    //char message[190];
    //sprintf(message, "M: %d \tCX: %ld; SX: %d; TX: %d \tCY: %ld; SY: %d; TY: %d", moveCount, stepperX.currentPosition(), slowDrawProgressX, TargetPositionArrTriangleX[moveCount], stepperY.currentPosition(), slowDrawProgressX, TargetPositionArrTriangleX[moveCount]);
    //Serial.println(message);
    const int stepUnits = 5;

    if(TargetPositionArrTriangleX[moveCount]<slowDrawProgressX) slowDrawProgressX -= stepUnits;
    else if (TargetPositionArrTriangleX[moveCount]>slowDrawProgressX) slowDrawProgressX += stepUnits;
    if(TargetPositionArrTriangleY[moveCount]<slowDrawProgressY) slowDrawProgressY -= stepUnits;
    else if (TargetPositionArrTriangleY[moveCount]>slowDrawProgressY) slowDrawProgressY += stepUnits;

    stepperX.moveTo(slowDrawProgressX);
    stepperY.moveTo(slowDrawProgressY);
  }  
}

void xyScan(){
  // Slow X scan
  // Check to see if the stepper has reached the target:
  if(stepperX.distanceToGo() == 0){
    if(xClockwise == true){
      xClockwise = false;  // move counterclockwise
      stepperX.moveTo(-20); // go back to the "home" (original) position
    } else {
      xClockwise = true;   // move clockwise
      stepperX.moveTo(100);  // go to the target position
    }
  }

  // Fast Y scan
  // Check to see if the stepper has reached the target:
  if(stepperY.distanceToGo() == 0){
    if(yClockwise == true){
      yClockwise = false;  // move counterclockwise
      stepperY.moveTo(0);  // go back to the "home" (original) position
    } else {
      yClockwise = true;   // move clockwise
      stepperY.moveTo(100);  // go to the target position
    }
  }
}
//*/
void xStepperHome(){ //this routine should run the motor
  hMinVal = digitalRead(xMin);
  while (hMinVal == LOW)
  {
    //backwards slowly till it hits the switch and stops
    stepperX.moveTo(4096);
    stepperX.run();
    hMinVal = digitalRead(xMin);
  }
  stepperX.setCurrentPosition(0); //set current motor position to zero
  stepperX.moveTo(-580-247);
  while (stepperX.distanceToGo() != 0){
    stepperX.run();
  }
  stepperX.setCurrentPosition(0); //set current motor position to zero
}

void yStepperHome(){ //this routine should run the motor
  hMinVal = digitalRead(yMin);
  while (hMinVal == LOW)
  {
    //backwards slowly till it hits the switch and stops
    stepperY.moveTo(4096);
    stepperY.run();
    hMinVal = digitalRead(yMin);
  }
  stepperY.setCurrentPosition(0); //set current motor position to zero
  stepperY.moveTo(-900+175);
  while (stepperY.distanceToGo() != 0){
    stepperY.run();
  }
  stepperY.setCurrentPosition(0); //set current motor position to zero
}

