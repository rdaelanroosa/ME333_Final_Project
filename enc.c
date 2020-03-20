#include "enc.h"

volatile static int pos = 0;
volatile static int vel = 0;
static int LastB;

void __ISR(26, IPL6SRS) EncController(void) {
    int a = PORTCbits.RC13;
    int b = PORTCbits.RC14;
    if (a == LastB) {
        pos++;
    } else {
        pos--;
    }

    LastB = b;

    LATDbits.LATD4 = (a == b);
    
    IFS1bits.CNIF = 0;
}

void enc_init() {
    TRISCbits.TRISC13 = 1;     //Set C13 and C14 as inputs
    TRISCbits.TRISC14 = 1;
    TRISDbits.TRISD4 = 0;

    LATDbits.LATD4 = 0;

    CNCONbits.ON = 1;
    CNENCLR = 0xFFFFFFFF;
    CNENbits.CNEN0 = 1;
    CNENbits.CNEN1 = 1;

    LastB = PORTCbits.RC14;

    IPC6bits.CNIP = 6;
    IPC6bits.CNIS = 0;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void enc_reset() {
    pos = 0;
}

int enc_pos_get() {
    return pos;
}

int enc_vel_get() {
    return vel;
}
