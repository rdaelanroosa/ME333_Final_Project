#include "pcon.h"
#include "utilities.h"
#include "encoder.h"
#include "NU32.h"

#define TPCON 6250
#define IMAX 1023
#define IMIN 0 
#define INTMAX 512
#define ITARE 508

static pPID pcon_gains = {0, 0, 100};
static int eint, eintmax, eprev, t, pticks, pdir, ediffprev;
char buffer[50];

static int get_PID() {
    int n = encoder_ticks();
    int e = t - n;
    eint += e;
    int edot = e - eprev;
    eprev = e;

    if (abs(eint) > eintmax) {
        eint = eintmax * (abs(eint) / eint);
    }

    int u = (pcon_gains.Kp * e) + (pcon_gains.Ki * eint) + (pcon_gains.Kd * edot) + ITARE;

    if (u > IMAX) {
        u = IMAX;
    } else if (u < IMIN) {
        u = IMIN;
    }
    //sprintf(buffer, "%d %d %d %d %d\r\n", u, mode_get(), e, n, t);
    //NU32_WriteUART3(buffer);
    return u;
    
}

void __ISR(_TIMER_4_VECTOR, IPL3SOFT) pController(void) {
    switch (mode_get()) {
        
        case HOLD: {
            icon_set_targ(get_PID());
            break;
        }

        case TRACK: {
            ;
        }

        default: {
            break;
        }
    }

    // Clear interrupt flag
    IFS0bits.T4IF = 0;
}


void pcon_init() {

    //Config Timer4 to trigger interrupt @ 200Hz for PWM duty cycle update ISR
    T4CONbits.TCKPS = 0b110;  
    PR4 = TPCON;
    TMR4 = 0;
    IPC4bits.T4IP = 3;
    IPC4bits.T4IS = 1;
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;

    T4CONbits.ON = 1;
}

void pcon_set_gains(float Kp, float Ki, float Kd) {
    int eintmaxtemp = INTMAX / Ki;

    __builtin_disable_interrupts();
    pcon_gains.Kp = Kp;
    pcon_gains.Ki= Ki;
    pcon_gains.Kd = Kd;
    eintmax = eintmaxtemp;
    __builtin_enable_interrupts();
}

pPID * pcon_get_gains() {
    return &pcon_gains;
}

void pcon_set_targ(int pos) {
    
    if (pos > 65535) {
        pos = 65535;
    } else if (pos < 0) {
        pos = 0;
    }
    __builtin_disable_interrupts();
    t = pos;
    __builtin_enable_interrupts();
}