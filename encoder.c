#include "encoder.h"
#include "util.h"
#include "NU32.h"
#include <xc.h>

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

int encoder_get() {
    encoder_command(1);
    int pos = encoder_command(1);
    Mode mode = util_mode_get();
    if ((pos == 0) && ((mode == HOLD) || (mode == TRACK))) { 
        NU32_WriteUART3("ENCODER THROWING TO IDLE \r\n");
        util_mode_set(IDLE);
    }
    return pos;
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
