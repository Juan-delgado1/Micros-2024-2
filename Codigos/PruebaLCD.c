#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
void main(void){
    //ADCON1 = 15; //Desactivar funciones analogas del puerto A
    //TRISA = 0;
    TRISD=0;
    //LATA = 0;
    LATD=0;
    __delay_ms(1000);
    ConfiguraLCD(4); //Modo de conexion
    InicializaLCD();
    //MensajeLCD_Var("Hola mundo");
    EscribeLCD_c('H');
    EscribeLCD_c('o');
    EscribeLCD_c('l');
    EscribeLCD_c('a');
    EscribeLCD_c(' ');
    EscribeLCD_c('m');
    EscribeLCD_c('u');
    EscribeLCD_c('n');
    EscribeLCD_c('d');
    EscribeLCD_c('o');    
    DireccionaLCD(0xC0);
    EscribeLCD_c('7');
    EscribeLCD_c(';');
    while(1){
    }    
}