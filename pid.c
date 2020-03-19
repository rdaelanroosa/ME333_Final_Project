#include "pid.h"
#include "NU32.h"

int pid_get(PIDObj * obj, int n) {
    int e = obj->t - n;
    obj->eint += e;
    int edot = e - obj->eprev;
    obj->eprev = e;

    //anti-windup
    if (obj->eint > obj->eintmax) {
        obj->eint = obj->eintmax;
    } else if (obj->eint < obj->eintmin) {
        obj->eint = obj->eintmin;
    }

    int u = (obj->Kp * e) + (obj->Ki * obj->eint) + (obj->Kd * edot) + obj->ucenter;

    //clamp output
    if (u > obj->umax) {
        u = obj->umax;
    } else if (u < obj->umin) {
        u = obj->umin;
    }

    return u;
}

void pid_set_targ(PIDObj * obj, int t) {
    int temp;
    if (t > obj->tmax) {
        temp = obj->tmax;
    } else if (t < obj->tmin) {
        temp = obj->tmin;
    } else {
        temp = t;
    }

     __builtin_disable_interrupts();
    obj->t = temp;
    __builtin_enable_interrupts();
}

void pid_set_coeffs(PIDObj * obj, float Kp, float Ki, float Kd) {
    int temp1, temp2;
    if (Ki == 0) {
        temp1 = obj->umax / obj->Ki;
        temp2 = obj->umin / obj->Ki;
    } else {
        temp1 = 0;
        temp2 = 0;
    }

    __builtin_disable_interrupts();
    obj->Kp = Kp;
    obj->Ki = Ki;
    obj->Kd = Kd;
    obj->eintmax = temp1;
    obj->eintmin = temp2;
    __builtin_enable_interrupts();
    
}
