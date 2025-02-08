#include<xc.h>
#define _XTAL_FREQ 1000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

char conversion(void);
void transmitir(char);
void main(void){
    TRISA = 0;
    TRISD=0;
    LATA = 0;
    LATD=0;
    
    ADCON0 = 0b00000001;
    ADCON1 = 0b00000001;
    ADCON2 = 0b00001000;
    TXSTA = 0b00100000;     // TRANSMIT STATUS AND CONTROL REGISTER 0b00100x00;   
    RCSTA = 0b10000000;     // RECEIVE STATUS AND CONTROL REGISTER
    BAUDCON = 0b0000000;    // BAUD RATE CONTROL REGISTER   0b000x000;
    SPBRG = 25;

    while(1){
        transmitir('H');
        transmitir('o');
        transmitir('l');
        transmitir('a');
        transmitir('\r');
        __delay_ms(1000);
    }
}
char conversion(void){
    GO=1;
    while(GO ==1);
    return ADRESH;
}

void transmitir(char BufferT){
    while(TRMT == 0);
    TXREG=BufferT;
}

