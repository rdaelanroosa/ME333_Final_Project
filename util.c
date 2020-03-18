#include "util.h"

static Mode PICMode = IDLE;
// return operating mode
Mode util_mode_get() {
    return PICMode;
}

//set operating mode
void util_mode_set(Mode newMode) {
    PICMode = newMode;
}

// send test results to client
void util_return_data(DataPoint * results) {
    char buffer[50];
    int endflag = 0;
    int i = 0;
    while (!endflag) {
        sprintf(buffer, "%d %d %f %f %f\r\n", results[i].endflag, results[i].index, results[i].target, results[i].value, results[i].effort);
        NU32_WriteUART3(buffer);
        endflag = results[i].endflag;
        i++;
    }
}


// functions for unit conversion

float cnvtt_encoder_deg(int ticks) {
    return (((float) (ticks - TARE)) / PULSES) * DEGREES;
}

int cnvtt_encoder_ticks(float degrees) {
    return ((((float) degrees) / DEGREES) * PULSES) + TARE;
}

float cnvtt_isense_ma(int ticks) {
    return (ticks - CENTER) * TICKSTOMA;
}

int cnvtt_isense_ticks(float mA) {
    return (mA / TICKSTOMA) + CENTER;
}

float cnvtt_icon_pwm(int ticks) {
    return (((float) ticks) / TPWM) * 100.0;
}

int cnvtt_icon_ticks(float pwm) {
    return (pwm / 100.0) * TPWM;
}
