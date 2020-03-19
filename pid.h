#ifndef _PID_H
#define _PID_H

typedef struct {
    int t;
    int tmax;
    int tmin;
    int eprev;
    int eint;
    int eintmax;
    int eintmin;
    int ucenter;
    int umax;
    int umin;
    float Kp;
    float Ki;
    float Kd;
} PIDObj;

int pid_get(PIDObj *  obj, int n);

void pid_set_targ(PIDObj * obj, int t);

void pid_set_coeffs(PIDObj * obj, float Kp, float Ki, float Kd);

#endif