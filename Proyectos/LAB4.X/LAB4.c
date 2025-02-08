#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma config PBADEN=OFF
#pragma config LVP=OFF          //Desactivar RB5 bajo voltaje

unsigned char LeerTeclado(void);
void interrupt ISR(void);

void main(void){
    
}

unsigned char LeerTeclado(void){
    while(RB4==1 && RB5==1 && RB6==1 && RB7==1);
    LATB=0b11111110;
    if(RB4==0) return 1;
    if(RB5==0) return 2;
    if(RB6==0) return 3;
    if(RB7==0) return 4;
    LATB=0b11111101;
    if(RB4==0) return 5;
    if(RB5==0) return 6;
    if(RB6==0) return 7;
    if(RB7==0) return 8;
    LATB=0b11111011;
    if(RB4==0) return 9;
    if(RB5==0) return 10;
    if(RB6==0) return 11;
    if(RB7==0) return 12;
    LATB=0b11110111;
    if(RB4==0) return 13;
    if(RB5==0) return 14;
    if(RB6==0) return 15;
    if(RB7==0) return 16;
}

void interrupt ISR(void){
    if(RBIF==1){
        if(PORTB!=0b11110000){
            Tecla=0;
            LATB=0b11111110;
            if(RB4==0) Tecla=1;
            else if(RB5==0) Tecla=2;
            else if(RB6==0) Tecla=3;
            else if(RB7==0) Tecla=4;
            else{
                LATB=0b11111101;
                if(RB4==0) Tecla=5;
                else if(RB5==0) Tecla=6;
                else if(RB6==0) Tecla=7;
                else if(RB7==0) Tecla=8;
                else{
                    LATB=0b11111011;
                    if(RB4==0) Tecla=9;
                    else if(RB5==0) Tecla=10;
                    else if(RB6==0) Tecla=11;
                    else if(RB7==0) Tecla=12;
                    else{
                        LATB=0b11110111;
                        if(RB4==0) Tecla=13;
                        else if(RB5==0) Tecla=14;
                        else if(RB6==0) Tecla=15;
                        else if(RB7==0) Tecla=16;
                    }
                }
            }
            LATB=0b11110000;
        }
        __delay_ms(100);
        RBIF=0;
    }
    if(TMR0IF==1){
        TMR0IF=0;
        TMR0=3036;
        LATD1=LATD1^1;
    }
}