#ifndef ISENSE_H
#define ISENSE_H

void isense_init();

float isense_mA();

int isense_ticks();

float isense_ticks_mA(int ticks);

int isense_mA_ticks(float mA);

#endif