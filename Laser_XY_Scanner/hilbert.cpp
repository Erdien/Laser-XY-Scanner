#include "hilbert.h"
#include "Scanner_setup.h"

int numPoints;
int hilbertIterator = 0;
int *hilbertArrX;
int *hilbertArrY;
int x = -1, y = 0;  // Global variables.
int s = 0;          // Dist. along curve.

void hilbertInit(){
  int order=3;
  numPoints = pow(2, order * 2);
  hilbertArrX = (int*)calloc(numPoints, sizeof(int));
  hilbertArrY = (int*)calloc(numPoints, sizeof(int));
  step(0);
  hilbert(0, 1, order);
}

void step(int dir) {
  switch (dir & 3) {
    case 0: x = x + 1; break;
    case 1: y = y + 1; break;
    case 2: x = x - 1; break;
    case 3: y = y - 1; break;
  }
  hilbertArrX[s] = x;
  hilbertArrY[s] = y;
#ifdef MESSAGES
  char message[50];
  sprintf(message, "%5d \n%d %d %d", dir, s, x ,y);
  Serial.println(message);
#endif
  s = s + 1;  // Increment distance.
}
void hilbert(int dir, int rot, int order) {
  if (order == 0) return;
  dir = dir + rot;
  hilbert(dir, -rot, order - 1);
  step(dir);
  dir = dir - rot;
  hilbert(dir, rot, order - 1);
  step(dir);
  hilbert(dir, rot, order - 1);
  dir = dir - rot;
  step(dir);
  hilbert(dir, -rot, order - 1);
}
  
void hilbertRun(){
  if(stepperX.distanceToGo() == 0 and stepperY.distanceToGo() == 0){
    int scale=5;
    delay(20);
    stepperX.moveTo(hilbertArrX[hilbertIterator]*scale);
    stepperY.moveTo(hilbertArrY[hilbertIterator]*scale);
#ifdef MESSAGES
    char message[50];
    sprintf(message, "Target x: %d; Target y: %d", hilbertArrX[hilbertIterator]*scale, hilbertArrY[hilbertIterator]*scale);
    Serial.println(message);
#endif
    hilbertIterator++;
    hilbertIterator %= numPoints-1;
  }
}
