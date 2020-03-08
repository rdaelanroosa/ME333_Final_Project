#ifndef ICON_H
#define ICON_H

#include "utilities.h"

typedef struct {
    float Kp; 
    float Ki; 
} iPI;

void icon_init();

void icon_set_targ(int i);

void icon_set_gains(float Kp, float Ki);

iPI * icon_get_gains();

void icon_set_PWM(float power);

void icon_set_ticks(int ticks);

iTestDatum * icon_get_results();

#endif