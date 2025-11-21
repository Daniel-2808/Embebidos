#define _XTAL_FREQ 8000000
#include <xc.h>


void main(void){

    // Salidas de los segmentos
    TRISAbits.TRISA1 = 0; // a
    TRISAbits.TRISA2 = 0; // b
    TRISAbits.TRISA3 = 0; // c
    TRISEbits.TRISE2 = 0; // d
    TRISAbits.TRISA5 = 0; // e
    TRISEbits.TRISE0 = 0; // f
    TRISEbits.TRISE1 = 0; // g

    // Entrada del botón
    TRISCbits.TRISC0 = 1;

    // Tabla de segmentos (a b c d e f g)
    const unsigned char nums[10] = {
        0b1111110, // 0
        0b0110000, // 1
        0b1101101, // 2
        0b1111001, // 3
        0b0110011, // 4
        0b1011011, // 5
        0b1011111, // 6
        0b1110000, // 7
        0b1111111, // 8
        0b1111011  // 9
    };

    unsigned char digit = 0;

    while(1){

        unsigned char n = nums[digit];

        PORTAbits.RA1 = (n >> 6) & 1; // a
        PORTAbits.RA2 = (n >> 5) & 1; // b
        PORTAbits.RA3 = (n >> 4) & 1; // c
        PORTEbits.RE2 = (n >> 3) & 1; // d
        PORTAbits.RA5 = (n >> 2) & 1; // e
        PORTEbits.RE0 = (n >> 1) & 1; // f
        PORTEbits.RE1 = (n >> 0) & 1; // g

        if(PORTCbits.RC0 == 1){
            __delay_ms(30);
            if(PORTCbits.RC0 == 1){
                digit++;
                if(digit >= 10) digit = 0;
                while(PORTCbits.RC0 == 1);
                __delay_ms(30);
            }
        }
    }
}