#include "icon.h"
#include "isense.h"
#include "NU32.h"
#include "utilities.h"

#define TPWM 3999.0
#define NPTSITEST 100
#define ITESTH 200.0
#define ITESTL -200.0

#ifndef TICKSTOMA
#define TICKSTOMA 6.77  
#endif

static volatile iPI icon_gains;
static volatile int eint, eintmax, t, pticks, pdir;
static volatile float Waveform[NPTSITEST];
volatile iTestDatum iTestData[NPTSITEST];
char buffer[50];

static int get_PI(int n){
    int e = t - n;
    eint += e;

    if (abs(eint) > eintmax) {
        eint = eintmax * (abs(eint) / eint);
    }

    int u = (icon_gains.Kp) * e + (icon_gains.Ki * eint);
    
    if (abs(u) > TPWM) {
        u = TPWM * (abs(u) / u);
    }

    return u;

}

void __ISR(_TIMER_2_VECTOR, IPL3SOFT) iController(void) {
    static int ntest = 0;
    int i = isense_ticks();
    float m = isense_mA();
    int u;

    switch (mode_get()) {
        
        case IDLE: {
            OC1RS = 0;
            break;
        }
        
        case PWM: {
            OC1RS = pticks;
            LATDbits.LATD1 = pdir;
            break;
        }

        case HOLD: {
            OC1RS = get_PI(i);
        }

        case ITEST: {
            sprintf(buffer, "\r\nNew targ (mA) %f\r\n", Waveform[ntest]);
            NU32_WriteUART3(buffer);
            sprintf(buffer, "\r\nNew targ (ticks) %d\r\n", isense_mA_ticks(Waveform[ntest]));
            NU32_WriteUART3(buffer);
            icon_set_targ(isense_mA_ticks(Waveform[ntest]));
            int u = get_PI(i);
            OC1RS = u;
            iTestData[ntest].t = Waveform[ntest];
            iTestData[ntest].i = m;
            iTestData[ntest].o = (((float) u) / TPWM) * 100;
            iTestData[ntest].index = ntest;
            
            ntest++;
            if (ntest == NPTSITEST) {
                iTestData[ntest-1].endflag = 1;
                ntest = 0;
                mode_set(IDLE);
            } else {
                iTestData[ntest-1].endflag = 0;
            }
            break;
        }

        default: {
            break;
        }
    }

    // Clear interrupt flag
    IFS0bits.T2IF = 0;
}

void icon_init() {

    icon_set_gains(0, 0);

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

void icon_set_targ(int i) {

    __builtin_disable_interrupts();
    t = i;
    __builtin_enable_interrupts();

}

void icon_set_gains(float Kp, float Ki) {

    __builtin_disable_interrupts();
    icon_gains.Kp = Kp;
    icon_gains.Ki = Ki;
    eint = 0;
    __builtin_enable_interrupts();
}

iPI * icon_get_gains() {
    return &icon_gains;
}

void icon_set_PWM(float power) {

    power = (power / 100.0) * TPWM;

    int ptickstemp = abs((int) (power));

    if (ptickstemp > TPWM) {
        ptickstemp = TPWM;
    }

    if (ptickstemp < -TPWM) {
        ptickstemp = -TPWM;
    }

    int pdirtemp = (power < 0);
    
    __builtin_disable_interrupts();
    pticks = ptickstemp;
    pdir = pdirtemp;
    __builtin_enable_interrupts();
}

void icon_set_ticks(int ticks) {
    int ptickstemp = abs(ticks);
    if (ptickstemp > TPWM) {
        ptickstemp = TPWM;
    }

    int pdirtemp = (ticks < 0);
    
    __builtin_disable_interrupts();
    pticks = ptickstemp;
    pdir = pdirtemp;
    __builtin_enable_interrupts();

}

iTestDatum * icon_get_results() {
    return iTestData;
}