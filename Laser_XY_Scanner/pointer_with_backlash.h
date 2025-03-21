// https://github.com/Trytu035/anti_backlash
#ifndef BACKLASHED_POINTER
#define BACKLASHED_POINTER

#include "main2.h"

class PointerWithBacklash{
  private:
    Pointer &p;
    int maxBacklash;
    int backlash;
    int moveByWithBacklash(int value);
    int moveToWithBacklash(int value);
  public:
    PointerWithBacklash(Pointer &pointer, int maxBacklash);
    int moveByCounteractBacklash(int value);
    int moveToCounteractBacklash(int value);
    int getValue();
};

#endif