//Fosc=1MHz
//BaudRate 9600 bps
#include<xc.h>
#define _XTAL_FREQ 1000000
////#include "LibLCDXC8.h"          // Libreria LCD

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF


void Transmitir(unsigned char);

unsigned char Recibir(void);

void main(void){
    //unsigned char BufferR;
    unsigned char contador = 0;
    TRISD = 0;
    LATD = 0;
    TXSTA=0b00000100; //brgh = 1 , SYN = 0 , ten = 0
    RCSTA=0b10010000;       // CREN = 1
    BAUDCON=0b00001000;  //brgh16 = 1      
    SPBRG=25;               //9600 bps
    while(1){
        
        /*
        Transmitir('H');
        Transmitir('o');
        Transmitir('l');
        Transmitir('a');
        Transmitir(' ');
        Transmitir('m');
        Transmitir('u');
        Transmitir('n');
        Transmitir('d');
        Transmitir('o');               
        __delay_ms(500);
        //BufferR=Recibir(); */ 
        //LATD = Recibir();
        Transmitir(1);
    }
}

unsigned char Recibir(void){
    while(RCIF==0);
    return RCREG;
}

void Transmitir(unsigned char BufferT){
    while(TRMT==0);
    TXREG=BufferT+48 ;
}
