#include "isense.h"
#include <xc.h>

#define TICKSTOMA 6.77
#define MATOTICKS 0.1477
#define CENTER 508
#define NAVG 100

 

void isense_init(){
    
    // Confic ADC to read pin RB0 as analog with auto sample, auto comversion
    AD1PCFGbits.PCFG0 = 1;
    AD1CON1bits.SSRC = 0b111;
    AD1CON1bits.ASAM = 1;
    AD1CON2bits.VCFG = 0b000;
    AD1CON3bits.ADCS  = 255;    // Tuned to minimize noise
    AD1CON3bits.SAMC = 31;      // Tuned to minimize noise
    AD1CON2bits.SMPI = 0;
    AD1CON1bits.ON = 1;


}

float isense_mA() {
    return (isense_ticks() - CENTER) * TICKSTOMA;
}

int isense_ticks() {
    AD1CHSbits.CH0SA = 0;    //Assumes pin previously configured for analog input
    return ADC1BUF0;
}

float isense_ticks_mA(int ticks) {
    return ((float) (ticks - CENTER)) * TICKSTOMA;
}

int isense_mA_ticks(float mA) {
    return ((int) (mA * MATOTICKS)) + CENTER;
}