#include "NU32.h"
#include <stdio.h>
#include "encoder.h"
#include "icon.h"
#include "pcon.h"
#include "util.h"

#define BUF_SIZE 200

int main() {

    char buffer[BUF_SIZE];
    float Kp, Ki, Kd, newtarg;
    iPI * igainsp;
    pPID * pgainsp;

    util_mode_set(IDLE);
    encoder_init();
    isense_init();
    icon_init();
    pcon_init();
    
    
    
    NU32_Startup();
    NU32_LED1 = 0;
    NU32_LED2 = 1;

    __builtin_disable_interrupts();

    // Config Peripherals

    __builtin_enable_interrupts();

    while(1) {
        NU32_ReadUART3(buffer, BUF_SIZE);
        NU32_LED2 = 1;
        NU32_LED1 = 1;
        switch (buffer[0]) {

            case 'a': {
                sprintf(buffer, "%d\n", isense_ticks());
                NU32_WriteUART3(buffer);
                break;
            }

            case 'b': {
                sprintf(buffer, "%f\n", cnvtt_isense_ma(isense_ticks()));
                NU32_WriteUART3(buffer);
                break;
            }

            case 'c': {
                sprintf(buffer, "%d\n", encoder_get());
                NU32_WriteUART3(buffer);
                break;
            }
            
            case 'd': {
                sprintf(buffer, "%f\n", cnvtt_encoder_deg(encoder_get())); 
                NU32_WriteUART3(buffer);
                break;
            }

            case 'e': {
                encoder_reset();
                break;
            }

            case 'f': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                float setval;
                sscanf(buffer, "%f\n", &setval);
                icon_set(cnvtt_icon_ticks(setval));
                util_mode_set(PWM);
                sprintf(buffer, "%f\n", setval);
                NU32_WriteUART3(buffer);
                break;
            }

            case 'g': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f %f\n", &Kp, &Ki);
                icon_set_gains(Kp, Ki);
            }

            case 'h': {
                igainsp = icon_get_gains();
                sprintf(buffer, "%f %f\n", igainsp->Kp, igainsp->Ki);
                NU32_WriteUART3(buffer);
                break;
            }

            case 'i': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f %f %f\n", &Kp, &Ki, &Kd);
                pcon_set_gains(Kp, Ki, Kd);
                pgainsp = pcon_get_gains();
                sprintf(buffer, "%f %f\n", igainsp->Kp, igainsp->Ki);

            }

            case 'j': {
                pgainsp = pcon_get_gains();
                sprintf(buffer, "%f %f %f\n", pgainsp->Kp, pgainsp->Ki, pgainsp->Kd);
                NU32_WriteUART3(buffer);
                break;
            }

            case 'k': {
                DataPoint * idatap;
                int i = 0;
                int endflag = 0;

                util_mode_set(ITEST);
                
                while (util_mode_get() != IDLE) {
                    ;
                }

                util_return_data(icon_get_results());

                break;
            }
            
            case 'l': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f\n", &newtarg);
                pcon_set_targ(cnvtt_encoder_ticks(newtarg));
                util_mode_set(HOLD);
                break;
            }

            case 'm': {
                
            }

            case 'n': {
                int numentries;
                int * traj;
                float temp;
                util_mode_set(IDLE);
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d\n", &numentries);
                traj = pcon_get_traj(numentries);
                int i = 0;

                while (i < numentries) {
                    NU32_ReadUART3(buffer, BUF_SIZE);
                    sscanf(buffer, "%f\n", &temp);
                    sprintf(buffer, "%d %f\r\n", cnvtt_encoder_ticks(temp), cnvtt_encoder_deg(cnvtt_encoder_ticks(temp)));
                    NU32_WriteUART3(buffer);
                    traj[i] = cnvtt_encoder_ticks(temp);
                    i++;
                }

                break;
                
            }
            
            case 'o': {
                encoder_reset();
                
                util_mode_set(TRACK);
                while (util_mode_get() == TRACK) {;}
                util_return_data(pcon_get_results());
                break;
            }

            case 'p': {
                util_mode_set(IDLE);
                break;
            }

            case 'q': {
                util_mode_set(IDLE);
                sprintf(buffer, "%d\r\n", util_mode_get());
                NU32_WriteUART3(buffer);
                break;
            }

            case 'r': {
                sprintf(buffer, "%d\r\n", util_mode_get());
                NU32_WriteUART3(buffer);
                break;
            }

            case 't': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                //float setval = strtof(buffer, &buffer);
                int setval;
                sscanf(buffer, "%d", &setval);
                icon_set(setval);
                util_mode_set(PWM);
                sprintf(buffer, "%d\n", setval);
                NU32_WriteUART3(buffer);
                break;
            }

            default: {
                NU32_LED2 = 0; //ERROR
                break;
            }
        }
    }

    return 0;
}