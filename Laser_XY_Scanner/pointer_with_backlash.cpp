#include "pointer_with_backlash.h"

int clamp(int x, int min, int max){
  if (x < min){
      return min;
  } else if (x > max){
      return max;
  }else{
      return x;
  }
}

PointerWithBacklash::PointerWithBacklash(Pointer &pointer, int maxBacklash) : p(pointer), maxBacklash(maxBacklash), backlash(0) {}


int PointerWithBacklash::moveByWithBacklash(int value){
  // TODO:
  // assert, that pointer is initialized (haven't done that yet)
  int ret = this->p.moveBy(value);
  if (0 == ret){
      this->backlash = clamp(this->backlash + value, -this->maxBacklash, this->maxBacklash);
  }
  
  return ret;
}

int PointerWithBacklash::moveByCounteractBacklash(int value){
  if (value > 0){
      value += this->maxBacklash - this->backlash;
  }else if (value < 0){
      value -= this->maxBacklash + this->backlash;
  }
  
  return moveByWithBacklash(value);
}

int PointerWithBacklash::moveToWithBacklash(int value){
  // TODO:
  // assert, that pointer is initialized (haven't done that yet)
  int currVal = this->p.value;
  int ret = this->p.moveTo(value);

  if (0 == ret){
      this->backlash = clamp(this->backlash - currVal + value, 0, this->maxBacklash);
  }
  
  return ret;
}

int PointerWithBacklash::moveToCounteractBacklash(int value){
  int currentValue = this->p.value - this->backlash;

  if (value - currentValue > 0){
      value += this->maxBacklash;
  }else if (value - currentValue < 0){
  //}else if (value - currentValue < 0){
      //value -= this->maxBacklash;
  }else {
      value += this->backlash;
  }

  return moveToWithBacklash(value);
}

int PointerWithBacklash::getValue(){
  return this->p.value - this->backlash;
}