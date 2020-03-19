#ifndef PCON_H
#include "util.h"
#include "pid.h"

void pcon_set_pos_targ(int pos);

void pcon_set_pos_gains(float Kp, float Ki, float Kd);

Gains pcon_get_pos_gains();

void pcon_set_vel_targ(int pos);

void pcon_set_vel_gains(float Kp, float Ki);

Gains pcon_get_vel_gains();

int * pcon_get_traj();

DataPoint * pcon_get_results();

#endif