#ifndef ICON_H
#define ICON_H

typedef struct {
    float Kp; 
    float Ki; 
} iPI;

void icon_init();

void icon_set_targ(int i);

void icon_set_gains(float Kp, float Ki);

int icon_set_PWM(float power);

void icon_set_ticks(int ticks);

#endif