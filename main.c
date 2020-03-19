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
    float * gains;

    //Startup, set status lights to indicate reboot (for debugging segfaults and other runtime errors)
    NU32_Startup();
    NU32_LED1 = 0;
    NU32_LED2 = 1;

    __builtin_disable_interrupts();

    //Init modules
    util_mode_set(IDLE);
    encoder_init();
    isense_init();
    icon_init();
    pcon_init();

    __builtin_enable_interrupts();

    while(1) {
        NU32_ReadUART3(buffer, BUF_SIZE);  //read menu item
        NU32_LED2 = 1;  // Clear status lights
        NU32_LED1 = 1;

        switch (buffer[0]) {

            // read current in ticks
            case 'a': {
                sprintf(buffer, "%d\n", isense_ticks());
                NU32_WriteUART3(buffer);
                break;
            }

            //read current in mA
            case 'b': {
                sprintf(buffer, "%f\n", cnvtt_isense_ma(isense_ticks()));
                NU32_WriteUART3(buffer);
                break;
            }

            // read position in ticks
            case 'c': {
                sprintf(buffer, "%d\n", encoder_get());
                NU32_WriteUART3(buffer);
                break;
            }
            
            //read position in degrees
            case 'd': {
                sprintf(buffer, "%f\n", cnvtt_pos_deg(encoder_get())); 
                NU32_WriteUART3(buffer);
                break;
            }
            
            //reset encoder
            case 'e': {
                encoder_reset();
                break;
            }
            
            //set pwm duty cycle
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

            // Set current gains
            case 'g': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f %f\n", &Kp, &Ki);
                icon_set_gains(Kp, Ki);
            }

            // read current gains
            case 'h': {
                gains = icon_get_gains();
                sprintf(buffer, "%f %f\n", igainsp->Kp, igainsp->Ki);
                NU32_WriteUART3(buffer);
                break;
            }
            
            // set position gains
            case 'i': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f %f %f\n", &Kp, &Ki, &Kd);
                pcon_set_gains(Kp, Ki, Kd);
                pgainsp = pcon_get_gains();
                sprintf(buffer, "%f %f\n", igainsp->Kp, igainsp->Ki);

            }

            // read position gains
            case 'j': {
                pgainsp = pcon_get_gains();
                sprintf(buffer, "%f %f %f\n", pgainsp->Kp, pgainsp->Ki, pgainsp->Kd);
                NU32_WriteUART3(buffer);
                break;
            }

            // run current gain test
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
            
            // hold bar at angle
            case 'l': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f\n", &newtarg);
                pcon_set_targ(cnvtt_pos_ticks(newtarg));
                util_mode_set(HOLD);
                break;
            }


            // read trajectory (falls through intentionally to next case)
            case 'm': {;}
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
                    sprintf(buffer, "%d %f\r\n", cnvtt_pos_ticks(temp), cnvtt_pos_deg(cnvtt_pos_ticks(temp)));
                    NU32_WriteUART3(buffer);
                    traj[i] = cnvtt_pos_ticks(temp);
                    i++;
                }

                break;
                
            }
            
            //execute trajectory
            case 'o': {
                encoder_reset();


                util_mode_set(TRACK);
                while (util_mode_get() == TRACK) {;}
                util_return_data(pcon_get_results());
                break;
            }

            //disable motor
            case 'p': {
                util_mode_set(IDLE);
                break;
            }

            //disable motor and confirm
            case 'q': {
                util_mode_set(IDLE);
                sprintf(buffer, "%d\r\n", util_mode_get());
                NU32_WriteUART3(buffer);
                break;
            }

            //get mode
            case 'r': {
                sprintf(buffer, "%d\r\n", util_mode_get());
                NU32_WriteUART3(buffer);
                break;
            }
            
            // Set status light to indicate unrecognized command
            default: {
                NU32_LED2 = 0; //ERROR
                break;
            }
        }
    }

    return 0;
}