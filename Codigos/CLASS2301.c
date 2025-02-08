#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
//Orden tal como está
// Espacio para librerias

void main(void){
    ADCON1 = 15; //Desactivar funciones analogas del puerto A
    TRISA = 0;
    TRISD=0;
    LATA = 0;
    LATD=0;
    __delay_ms(1000);
    ConfiguraLCD(4);
    InicializaLCD();
    EscribeLCD_c('H');
    EscribeLCD_c('i');
    while(1){
        EscribeLCD_n8(i,1);
        i++;
        __delay_ms(1000);
    }
}