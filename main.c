#include "NU32.h"
#include "encoder.h"
#include "icon.h"
#include "utilities.h"

#define BUF_SIZE 200

int main() {

    char buffer[BUF_SIZE];
    float Kp, Ki, Kd;
    iPI * igainsp;

    mode_set(IDLE);
    encoder_init();
    icon_init();
    isense_init();
    
    
    
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
                sprintf(buffer, "%f\n", isense_mA());
                NU32_WriteUART3(buffer);
            
                break;
            }

            case 'c': {
                sprintf(buffer, "%d\n", encoder_ticks());
                NU32_WriteUART3(buffer);
                break;
            }
            
            case 'd': {
                sprintf(buffer, "%f\n", encoder_degrees()); 
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
                icon_set_PWM(setval);
                mode_set(PWM);
                sprintf(buffer, "%f\n", setval);
                NU32_WriteUART3(buffer);
                break;
            }

            case 'g': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f %f\n", &Kp, &Ki);
                icon_set_gains(Kp, Ki);
                igainsp = icon_get_gains();
                sprintf(buffer, "%f %f\n", igainsp->Kp, igainsp->Ki);
                NU32_WriteUART3(buffer);
                break;
            }

            case 'h': {
                igainsp = icon_get_gains();
                sprintf(buffer, "%f %f\n", igainsp->Kp, igainsp->Ki);
                NU32_WriteUART3(buffer);
                break;
            }

            case 'p': {
                mode_set(IDLE);
                break;
            }

            case 'q': {
                mode_set(IDLE);
                sprintf(buffer, "%d\r\n", mode_get());
                NU32_WriteUART3(buffer);
                break;
            }

            case 'r': {
                sprintf(buffer, "%d\r\n", mode_get());
                NU32_WriteUART3(buffer);
                break;
            }

            case 't': {
                NU32_ReadUART3(buffer, BUF_SIZE);
                //float setval = strtof(buffer, &buffer);
                int setval;
                sscanf(buffer, "%d", &setval);
                icon_set_ticks(setval);
                mode_set(PWM);
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