#include "pcon.h"
#include "util.h"
#include "encoder.h"
#include "NU32.h"

static pPID pcon_gains = {0, 0, 100};
static int eint, eintmax, eprev, t, pticks, pdir, ediffprev;
char buffer[50];
volatile int trajectory[MAXTRAJ];
volatile int trajlen;
volatile DataPoint pTestData[MAXTRAJ];

static int get_PID(int n) {
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

    return u;
    
}

void __ISR(_TIMER_4_VECTOR, IPL3SOFT) pController(void) {
    
    static int ntest = 0;
    static int pprev = 0;
    int p = encoder_get();
    int u;

    switch (util_mode_get()) {
    
        case HOLD: {
            u = get_PID(p);
            break;
        }

        case TRACK: {
            pcon_set_targ(trajectory[ntest]);
            u = get_PID(p);
            icon_set_targ(u);

            pTestData[ntest].target = cnvtt_encoder_deg(trajectory[ntest]);
            pTestData[ntest].value = cnvtt_encoder_deg(p);
            pTestData[ntest].effort = cnvtt_isense_ma(u);


            ntest++;
            if (ntest == trajlen) {
                pTestData[ntest-1].endflag = 1;
                ntest = 0;
                util_mode_set(IDLE);
            } else {
                pTestData[ntest-1].endflag = 0;
            }
            
            break;
        }

        case SPEED: {
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

int * pcon_get_traj(int numentries) {
    trajlen = numentries;
    return trajectory;
}

DataPoint * pcon_get_results() {
    return pTestData;
}
    