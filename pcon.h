#ifndef PCON_H
#include "util.h"

// PID gains struct
typedef struct {
    float Kp; 
    float Ki;
    float Kd; 
} pPID;

void pcon_set_targ(int pos);

int * pcon_get_traj();

pPID * pcon_get_gains();

DataPoint * pcon_get_results();

#endif