#ifndef UTILITIES_H

#define UTILITIES_H
#include <xc.h>


typedef enum Mode{
    IDLE, 
    PWM, 
    ITEST, 
    HOLD, 
    TRACK
} Mode;

typedef struct {
    int endflag;
    int index;
    float t; 
    float i; 
    float o;
} iTestDatum;


Mode mode_get();

void mode_set(Mode newMode);

#endif

