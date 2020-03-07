#include "isense.h"
#include <xc.h>

#define TICKSTOMA 6.08
#define CENTER 508

 

void isense_init(){
    
    // Confic ADC to read pin RB0 as analog with auto sample, auto comversion
    AD1PCFGbits.PCFG0 = 1;
    AD1CON1bits.SSRC = 0b111;
    AD1CON1bits.ASAM = 0;
    AD1CON2bits.VCFG = 0b00;
    AD1CON3bits.ADCS  = 15;    // Tuned to minimize noise
    AD1CON3bits.SAMC = 2;      // Tuned to minimize noise
    AD1CON2bits.SMPI = 0;
    AD1CON1bits.ON = 1;


}

float isense_mA() {
    return (isense_ticks() - CENTER) * TICKSTOMA;
}

int isense_ticks() {
    AD1CHSbits.CH0SA = 0;    //Assumes pin previously configured for analog input
    AD1CON1bits.SAMP = 1;
    while(!AD1CON1bits.DONE) {
        ;
    }
    return ADC1BUF0;
}