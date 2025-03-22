// https://github.com/Trytu035/anti_backlash
#ifndef RANGE_POINTER
#define RANGE_POINTER

#include <AccelStepper.h>

class Range{
public:
  int min1;
  int max1;
  Range(int min1, int max1);
};

class Pointer{
public:
  int value;
  Range &range;
  AccelStepper &actuator;
  Pointer(Range &range, AccelStepper &actuator);
  int moveBy(int x);
  int moveTo(int x);
  int getValue();
};


#endif