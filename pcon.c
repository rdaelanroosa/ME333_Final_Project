#include "pcon.h"
#include "util.h"
#include "pid.h"
#include "encoder.h"
#include "NU32.h"

static PIDObj pos_pid = {
    PCENTER,
    PMAX,
    PMIN,
    0,
    0,
    0,
    0,
    ICENTER,
    IMAX,
    IMIN,
    PKPBASE,
    PKIBASE,
    PKDBASE
};

static PIDObj vel_pi = {
    0,
    VMAX,
    VMIN,
    0,
    0,
    (IMAX - ICENTER) / VKIBASE,
    (IMIN - ICENTER) / VKIBASE,
    ICENTER,
    IMAX,
    IMIN,
    VKPBASE,
    VKIBASE,
    0
};

volatile int trajectory[MAXTRAJ];
volatile int trajlen;
static volatile int vel, pos;
volatile DataPoint pTestData[MAXTRAJ];


// position control interrupt
void __ISR(_TIMER_4_VECTOR, IPL3SOFT) pController(void) {
    
    static int ntest = 0;
    static int posprev = 0;
    pos = encoder_get();
    vel = pos - posprev;
    int u;

    switch (util_mode_get()) {
    
        case HOLD: {
            
            // in hold mode pass PID output to current PI control
            icon_set_targ(pid_get(&pos_pid, pos));

            break;
        }

        case TRACK: {
            // Track trajectory with PID
            pcon_set_pos_targ(trajectory[ntest]);
            u = pid_get(&pos_pid, pos);
            icon_set_targ(u);

            //record data
            pTestData[ntest].target = cnvtt_pos_deg(trajectory[ntest]);
            pTestData[ntest].value = cnvtt_pos_deg(pos);
            pTestData[ntest].effort = cnvtt_isense_ma(u);

            //reset and go to IDLE at the end of the trajectory
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

        // not yet implemented
        case SPEED: {
            icon_set_targ(pid_get(&vel_pi, vel));
        }

        // do nothing in IDLE, PWM, ITEST
        default: {
            break;
        }
    }

    // Clear interrupt flag
    posprev = pos;
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

// Set PID gains
void pcon_set_pos_gains(float Kp, float Ki, float Kd) {
    pid_set_coeffs(&pos_pid, Kp, Ki, Kd);
}

//return pointer to PID gains
Gains pcon_get_pos_gains() {
    Gains gains = pid_get_gains(&pos_pid);
    return gains;
}

// set target for PID
void pcon_set_pos_targ(int pos) {
  pid_set_targ(&pos_pid, pos);
}

void pcon_set_vel_gains(float Kp, float Ki) {
    pid_set_coeffs(&vel_pi, Kp, Ki, 0);
}

//return array of PID gains
Gains pcon_get_vel_gains() {
    Gains gains = pid_get_gains(&vel_pi);
    return gains;
}

// set target for PID
void pcon_set_vel_targ(int vel) {
  pid_set_targ(&vel_pi, vel);
}

//return trajectory array
int * pcon_get_traj(int numentries) {
    trajlen = numentries;
    return trajectory;
}

//return gathered data
DataPoint * pcon_get_results() {
    return pTestData;
}
    