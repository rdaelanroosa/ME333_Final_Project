#include "encoder.h"
#include "utilities.h"
#include <xc.h>

#define PULSES 384.0
#define TARE 32768
#define DEGREES 360.0
#define RADIANS 6.283

char buffer[100];

static int encoder_command(int read) {
    SPI4BUF;
    SPI4BUF = read;
    while(!SPI4STATbits.SPIRBF)  {;}
    SPI4BUF;
    SPI4BUF = 5;
    while(!SPI4STATbits.SPIRBF)  {;}
    return SPI4BUF;
}

int encoder_deg_ticks(float degrees){
    float convert = ((degrees / DEGREES) * PULSES);
    int ticks = convert + TARE;
    return ticks;
}

int encoder_ticks() {
    encoder_command(1);
    int pos = encoder_command(1);
    if (pos == 0) {
        mode_set(IDLE);
    }
    return pos;
}

float encoder_degrees() {
    return ((encoder_ticks(1) - TARE) / PULSES) * DEGREES;
}

void encoder_reset() {
    encoder_command(0);
}

void encoder_init() {

    SPI4CON = 0;
    SPI4BUF;
    SPI4BRG = 0x4;
    SPI4STATbits.SPIROV = 0;
    SPI4CONbits.MSTEN = 1;
    SPI4CONbits.MSSEN = 1;
    SPI4CONbits.MODE16 = 1;
    SPI4CONbits.MODE32 = 0;
    SPI4CONbits.SMP = 1;
    SPI4CONbits.ON = 1;
}
