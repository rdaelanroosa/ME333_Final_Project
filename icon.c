#include "icon.h"
#include "isense.h"
#include "NU32.h"
#include "util.h"
#include "pid.h"

static volatile PIDObj current_pi = {
    ICENTER,
    IMAX,
    IMIN,
    0,
    0,
    TPWM / IKIBASE,
    -TPWM / IKIBASE,
    0,
    TPWM,
    -TPWM,
    IKPBASE,
    IKIBASE,
    0
};

static volatile float Waveform[NPTSITEST];
volatile DataPoint iTestData[NPTSITEST];

// current control interrupt
void __ISR(_TIMER_2_VECTOR, IPL3SOFT) iController(void) {
    static int ntest = 0;
    int i = isense_ticks();
    int u;

    switch (util_mode_get()) {
        
        //disable motor
        case IDLE: {
            //brake motor
            icon_set(0);
            break;
        }
        
        // do nothing; OC1RS is set by pwm command
        case PWM: {
            break;
        }

        case ITEST: {
            //track Waveform
            icon_set_targ(cnvtt_isense_ticks(Waveform[ntest]));
            u = pid_get(&current_pi, i);
            icon_set(u);

            //record data
            iTestData[ntest].target = Waveform[ntest];
            iTestData[ntest].value = cnvtt_isense_ma(i);
            iTestData[ntest].effort = cnvtt_icon_pwm(u);
            iTestData[ntest].index = ntest;
            
            // end test at end of Waveform, return to IDLE
            ntest++;
            if (ntest == NPTSITEST) {
                iTestData[ntest-1].endflag = 1;
                ntest = 0;
                util_mode_set(IDLE);
            } else {
                iTestData[ntest-1].endflag = 0;
            }
            break;
        }

        // track target set by pcon
        default: {
            icon_set(pid_get(&current_pi, i));
            break;
        }
    }

    // Clear interrupt flag
    IFS0bits.T2IF = 0;
}

void icon_init() {

    icon_set_gains(KPBASE, KIBASE);

    // init waveform
    int i;
    for (i = 0; i < NPTSITEST; i++) {
        if ((i < (NPTSITEST / 4)) | ((i >= (NPTSITEST / 2)) & (i < ((3*NPTSITEST)/4)))) {
            Waveform[i] = ITESTH;
        } else {
            Waveform[i] = ITESTL;
        }
        
    }

    //Config Timer2 to trigger interrupt @ 5kHz for PWM duty cycle update ISR
    T2CONbits.TCKPS = 0b110;  
    PR2 = 250;
    TMR2 = 0;
    IPC2bits.T2IP = 3;
    IPC2bits.T2IS = 0;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;

    //Config Timer3 to control 20kHz PWM signal from OC1 (NO interrupt)
    T3CONbits.TCKPS = 0;        
    PR3 = TPWM;                 
    TMR3 = 0;

    // Set D1 as output to control motor direction
    TRISDCLR = 2; 

    // Config OC1 to trigger off of Timer3 w/ PWM on D0
    OC1CONbits.OCM = 0b110;     // Set OC1 to PWM mode w/o fault pin
    OC1CONbits.OCTSEL = 1;      // Set OC1 to read from Timer3
    OC1RS = 0;               // duty cycle = OC1RS/(PR2 + 1) = 75%
    OC1R = 0; 

    T2CONbits.ON = 1;
    T3CONbits.ON = 1;
    OC1CONbits.ON = 1;

}

// set curret target
void icon_set_targ(int t) {
    pid_set_targ(current_pi, i);
}

// set PI gains
void icon_set_gains(float Kp, float Ki) {
    pid_set_coeffs(Kp, Ki, 0);
}

// return PI gains
float * icon_get_gains() {
    float gains[2] = {current_pi.Kp, current_pi.Ki};
    return gains;
}

//set motor output
void icon_set(int ticks) {
    int pticks = abs(ticks);
    if (pticks > TPWM) {
        pticks = TPWM;
    }

    int pdir = (ticks < 0);

    OC1RS = pticks;
    LATDbits.LATD1 = pdir;
}

//return itest results
DataPoint * icon_get_results() {
    return iTestData;
}