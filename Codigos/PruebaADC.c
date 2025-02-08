#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
unsigned int Conversion(unsigned char);
char conversion(void);
void main(void){
    unsigned int resultado1,resultado2;
    ADCON0=0b00000001;
    ADCON1=0b00000001
    ADCON2=0b00001000;
    TRISD=0; //Configurar puerto D como salida
    LATD=0;  // Inicializar
    /*
    __delay_ms(1000);
    ConfiguraLCD(4);
    InicializaLCD();
    MensajeLCD_Var("Hola mundo");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("Prueba ADC");
    __delay_ms(2000);
    */
    while(1){
        /*
        resultado1=Conversion(0);
        resultado2=Conversion(1);
        BorraLCD();
        EscribeLCD_n16(resultado1,4);
        DireccionaLCD(0xC0);
        EscribeLCD_n16(resultado2,4);
        __delay_ms(500);  
         */      
        LATD = conversion();
        __delay_ms(1000);
    }    
}
unsigned int Conversion(unsigned char canal){
    ADCON0=(ADCON0 & 0b00000011) | (canal<<2);
    GO=1;   //bsf ADCON0,1
    while(GO==1);
    return ADRES;
}

char conversion(void){
    GO = 1; //El solo se coloca en cero
    while(GO==1);
    return ADRESH; //devuelve lo que entrega el adc
}