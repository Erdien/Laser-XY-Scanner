#ifdef EXAMPLE
#include "example.h"
#include "Scanner_setup.h"

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
#endif