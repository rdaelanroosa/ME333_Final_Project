#include "utilities.h"

static Mode PICMode = IDLE;

Mode mode_get() {
    return PICMode;
}

void mode_set(Mode newMode) {
    PICMode = newMode;
}

