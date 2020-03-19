#ifndef ICON_H
#define ICON_H

#include "util.h"
#include "pid.h"

void icon_init();

void icon_set_targ(int i);

void icon_set_gains(float Kp, float Ki);

Gains icon_get_gains();

void icon_set_pwm(float power);

void icon_set(int ticks);

DataPoint * icon_get_results();

#endif