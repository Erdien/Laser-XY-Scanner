// https://github.com/lancetw/ebook-1/blob/master/02_algorithm/Hacker's%20Delight%202nd%20Edition.pdf
#ifndef __HILBERT_H
#define __HILBERT_H

#include <Arduino.h>

extern int *hilbertArrX;
extern int *hilbertArrY;

void hilbertInit();
void hilbertRun();
void step(int dir);
void hilbert(int dir, int rot, int order);

#endif
