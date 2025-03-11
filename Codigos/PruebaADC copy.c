#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
unsigned int ConversionADC2(unsigned char);
unsigned char ConversionADC(unsigned char);
char conversion(void);
void main(void){
    unsigned int resultado1,resultado2;
    unsigned char a;
    TRISD=0; //Configurar puerto D como salida
    LATD=0;  // Inicializar
    TRISC = 0;
    LATC = 0;   
    ADCON0=0b00000001;
    ADCON1=0b00001110;
    ADCON2=0b00001000;

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
        a = ConversionADC(0);
        unsigned char xd = a%10;
        LATD = a;
        //__delay_ms(1000);
    }    
}
unsigned char ConversionADC(unsigned char canal){
    if(canal > 12)
        return 0;
    
    ADCON0=(canal<<2)|0b00000001;
    GO=1;   //bsf ADCON0,1
    while(GO==1);
    return ADRESH;
}

unsigned int ConversionADC2(unsigned char canal){
    if(canal > 12)
        return 0;
    
    ADCON0=(canal<<2)|0b00000001;
    GO=1;   //bsf ADCON0,1
    while(GO==1);
    return ADRES;
}

char conversion(void){
    GO = 1; //El solo se coloca en cero
    while(GO==1);
    return ADRESH; //devuelve lo que entrega el adc
}