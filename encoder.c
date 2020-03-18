#include "encoder.h"
#include "util.h"
#include "NU32.h"
#include <xc.h>

// read and/or rest encoder chip
static int encoder_command(int read) {
    SPI4BUF;
    SPI4BUF = read;
    while(!SPI4STATbits.SPIRBF)  {;}
    SPI4BUF;
    SPI4BUF = 5;
    while(!SPI4STATbits.SPIRBF)  {;}
    return SPI4BUF;
}

// get motor position
int encoder_get() {
    //clear buffer
    encoder_command(1);
    
    //get position
    int pos = encoder_command(1);

    // if in encoder controlled mode, disable motor if the encoder saturates
    Mode mode = util_mode_get();
    if ((pos == 0) && ((mode == HOLD) || (mode == TRACK))) { 
        util_mode_set(IDLE);
    }
    return pos;
}

//reset encoder
void encoder_reset() {
    encoder_command(0);
}

//enable encoder chip over SPI
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
