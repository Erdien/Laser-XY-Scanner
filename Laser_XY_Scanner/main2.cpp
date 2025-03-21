#include "main2.h"

Range::Range(int min1, int max1) : min1(min1), max1(max1){
    /*this->min1 = min1;
    this->max1 = max1;*/
}


Pointer::Pointer(Range &range, AccelStepper &actuator) : value(0), range(range), actuator(actuator){
    /*this->value = 0;
    this->range = range;*/
}

//move by amount
int Pointer::moveBy(int value){
    int newVal = this->value + value;

    if (newVal > this->range.min1 && newVal < this->range.max1){
        this->value = newVal;
        return 0;
    }

    return -1;
}

int Pointer::moveTo(int value){
    // int relative_val = this->value - value;
    
    if (value > this->range.min1 && value < this->range.max1){
        this->value = value;
        actuator.moveTo(value);
        return 0;
    }
    
    return -1;
}

int Pointer::getValue(){
    return this->value;
}