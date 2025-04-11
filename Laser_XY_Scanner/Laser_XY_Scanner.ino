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
//#define EXAMPLE
//#define HILBERT
//#define MESSAGES
#define PACKET_SIZE 5

#include "Scanner_setup.h"

#ifdef HILBERT
  #include "hilbert.h"
#endif
#ifdef EXAMPLE
  #include "example.h"
#endif

#define MIN_X -500
#define MAX_X 500
#define MIN_Y -500
#define MAX_Y 500

#define HEADER 0x5e   //"^" dec 94
#define ACK 0x06
#define NACK 0x15

#define LASER 12            // define Laser switch output
char headerCounter = 0;

enum ErrorState {
  NO_ERROR = 0,
  OUT_OF_BOUNDS = 1 << 0,
  INVALID_HEADER = 1 << 1,
};

ErrorState stepperError;

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

  stepperInit(1000.0, 1000.0, 20000.0);

  //while(!Serial){}
  stepperError = NO_ERROR;


#ifdef HILBERT
  hilbertInit();
#endif

  digitalWrite(LASER, HIGH);       // turn on the laser
}

void loop() {

  // Uncomment only one drawing function at a time, then experiment to make your own!

  //calibrate();      // Helper method to manually steer motors with the help of serial. 
  serialControl();
  //hilbertRun();
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
    backlashX.moveToCounteractBacklash(targetX);
    backlashY.moveToCounteractBacklash(targetY);
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
          backlashX.moveToCounteractBacklash(targetX);
          backlashY.moveToCounteractBacklash(targetY);
          // Acknowledge master
          break;
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
}
