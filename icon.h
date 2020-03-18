#ifndef ICON_H
#define ICON_H

#include "util.h"

// PI gains struct
typedef struct {
    float Kp; 
    float Ki; 
} iPI;

void icon_init();

void icon_set_targ(int i);

void icon_set_gains(float Kp, float Ki);

iPI * icon_get_gains();

void icon_set_pwm(float power);

void icon_set(int ticks);

DataPoint * icon_get_results();

#endif