#include <xc.h>
#define _XTAL_FREQ 20000000 // 20MHz
#include "PIC16F877AConfig_4.h"

void pwm_init(void){
    
    TRISCbits.TRISC1 = 0;  
    TRISCbits.TRISC2 = 0;  
    TRISCbits.TRISC3 = 0;  

    // Modo PWM en CCP2
    CCP2CON = 0b00001100;  
    CCPR2L  = 0;          

    
    PR2     = 249;
   
    T2CON   = 0b00000100;
}


void set_percent(unsigned char percent){
    if (percent > 100) percent = 100;

    
    unsigned int max_counts = 4U * (PR2 + 1);
    unsigned int dc = (unsigned long)percent * max_counts / 100UL;

    CCPR2L = (dc >> 2);                

   
    CCP2CONbits.CCP2X = (dc >> 1) & 1;  
    CCP2CONbits.CCP2Y = dc & 1;         
}


void main (void) {

    unsigned char velocidad = 0;        
    unsigned char motor_activo = 0;     

    /
    unsigned char prevRB1 = 0;
    unsigned char prevRB2 = 0;
    unsigned char prevRB3 = 0;
    unsigned char prevRB4 = 0;
    unsigned char prevRB5 = 0;

   
    TRISB = 0b00111110;

    
    PORTCbits.RC1 = 0;   
    PORTCbits.RC2 = 0;   
    PORTCbits.RC3 = 0;   

    pwm_init();
    set_percent(0);     

    while(1){

        
        unsigned char currRB1 = PORTBbits.RB1;
        unsigned char currRB2 = PORTBbits.RB2;
        unsigned char currRB3 = PORTBbits.RB3;
        unsigned char currRB4 = PORTBbits.RB4;
        unsigned char currRB5 = PORTBbits.RB5;

        
        if (currRB2 && !prevRB2){
            motor_activo = 0;      
            set_percent(0);        
            __delay_ms(200);      
        }

       
        if (currRB4 && !prevRB4){
            PORTCbits.RC3 = 1;     // IN1
            PORTCbits.RC2 = 0;     // IN2

           
            if (velocidad > 0){
                motor_activo = 1;
                set_percent(velocidad);
            }
            __delay_ms(200);       
        }

       
        if (currRB5 && !prevRB5){
            PORTCbits.RC3 = 0;     // IN1
            PORTCbits.RC2 = 1;     // IN2

            if (velocidad > 0){
                motor_activo = 1;
                set_percent(velocidad);
            }
            __delay_ms(200);       // anti-rebote
        }

       
        if (currRB1 && !prevRB1){
            if (velocidad <= 90)
                velocidad += 10;
            else
                velocidad = 100;

            if (motor_activo){
                set_percent(velocidad);  
            }
            __delay_ms(200);            
        }

        
        if (currRB3 && !prevRB3){
            if (velocidad >= 10)
                velocidad -= 10;
            else
                velocidad = 0;

            if (motor_activo){
                set_percent(velocidad);
            } else {
                
            }
            __delay_ms(200);             
        }

       
        prevRB1 = currRB1;
        prevRB2 = currRB2;
        prevRB3 = currRB3;
        prevRB4 = currRB4;
        prevRB5 = currRB5;
    }
}

