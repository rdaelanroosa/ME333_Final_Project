#ifndef _ENCODER_H
#define _ENCODER_H

#include "NU32.h"
#include "util.h"

void enc_init(void);

int enc_pos_get(void);

int enc_vel_get(void);

void enc_reset(void);


#endif