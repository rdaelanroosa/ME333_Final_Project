#ifndef UTILITIES_H
#define UTILITIES_H

#include <xc.h>

//define project constants

// Macros from pcon.c
#define TPCON 6250
#define MAXTRAJ 2000
#define PKPBASE 5
#define PKIBASE 0
#define PKDBASE 50
#define VMAX 
#define VMIN
#define VKPBASE
#define VKIBASE

// Macros from icon.c
#define TPWM 3999.0         //PWM period
#define NPTSITEST 100
#define ITESTH 200.0
#define ITESTL -200.0
#define IKPBASE 19
#define IKIBASE 5
#define TICKSTOMA 6.77  

// Macros from encoder.c

#define PCENTER 32768
#define PMAX 65535
#define PMIN 0
#define PULSES 384.0
#define DEGREES 360.0
#define RADIANS 6.283

// Macros from isense.c
#define ICENTER 508
#define IMAX 1023
#define IMIN 0
#define NAVG 100

//conversion scaling constants
#define TICKSTOMA 6.77
#define MATOTICKS 0.1477

#define TICKSTODEG 0.9375
#define DEGTOTICKS 1.0667

#define TICKSTOPWM 39.99
#define PWMTOTICKS 0.025006

#define TICKSTORPM 31.25
#define RPMTOTICKS 0.032

typedef enum Mode{
    IDLE, 
    PWM,
    SPEED, 
    ITEST, 
    HOLD, 
    TRACK
} Mode;

typedef struct {
    int endflag;
    int index;
    float target; 
    float value; 
    float effort;
} DataPoint;

Mode util_mode_get();

void util_mode_set(Mode newMode);

void util_return_data(DataPoint * results);



float cnvtt_pos_deg(int ticks);

int cnvtt_pos_ticks(float degrees);

float cnvtt_isense_ma(int ticks);

int cnvtt_isense_ticks(float mA);

float cnvtt_icon_pwm(int ticks);

int cnvtt_icon_ticks(float pwm);

float cnvtt_vel_rpm(int ticks);

int cnvtt_vel_ticks(float rpm);

#endif

