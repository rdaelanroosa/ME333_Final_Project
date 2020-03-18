#ifndef ENCODER_H
#define ENCODER_H

void encoder_reset();

void encoder_init();

int encoder_get();

float encoder_degrees();

int encoder_deg_ticks(float degrees);

float encoder_ticks_deg(int ticks);

#endif