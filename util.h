#ifndef UTILITIES_H
#define UTILITIES_H

#include <xc.h>

//define project constants

// Macros from pcon.c
#define TPCON 6250
#define IMAX 1023
#define IMIN 0 
#define INTMAX 512
#define IRANGE 512
#define ITARE 508
#define MAXTRAJ 2000

// Macros from icon.c
#define TPWM 3999.0         //PWM period
#define NPTSITEST 100
#define ITESTH 200.0
#define ITESTL -200.0
#define KPBASE 19
#define KIBASE 5
#define TICKSTOMA 6.77  

// Macros from encoder.c
#define PULSES 384.0
#define TARE 32768
#define DEGREES 360.0
#define RADIANS 6.283

// Macros from isense.c
#define TICKSTOMA 6.77
#define MATOTICKS 0.1477
#define CENTER 508
#define NAVG 100


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

float cnvtt_encoder_deg(int ticks);

int cnvtt_encoder_ticks(float degrees);

float cnvtt_isense_ma(int ticks);

int cnvtt_isense_ticks(float mA);

float cnvtt_icon_pwm(int ticks);

int cnvtt_icon_ticks(float pwm);

#endif

