#include "util.h"

int pid_get(i)

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

float cnvtt_pos_deg(int ticks) {
    return ((float) (ticks - PCENTER)) * TICKSTODEG;
}

int cnvtt_pos_ticks(float degrees) {
    return ((int) (degrees * DEGTOTICKS)) + PCENTER;
}

float cnvtt_isense_ma(int ticks) {
    return ((float) (ticks - ICENTER)) * TICKSTOMA;
}

int cnvtt_isense_ticks(float mA) {
    return ((int) (mA * MATOTICKS)) + ICENTER;
}

float cnvtt_icon_pwm(int ticks) {
    return ((float) ticks) * TICKSTOPWM;
}

int cnvtt_icon_ticks(float pwm) {
    return ((int) (pwm * PWMTOTICKS));
}

float cnvtt_vel_rpm(int ticks) {
    return ((float) ticks) * TICKSTORPM;
}

int cnvtt_vel_ticks(float rpm) {
    return ((int) (rpm * RPMTOTICKS));
}
