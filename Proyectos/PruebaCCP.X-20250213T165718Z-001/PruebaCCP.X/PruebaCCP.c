#include<xc.h>
#define _XTAL_FREQ 1000000
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

void interrupt ISR(void);
void main(void){
    unsigned char i;
    TRISC1=0;
    TRISC2=0;
    TRISD=0;
    LATD=0;
    TMR2=0;
    PR2=249;    //Fpwm=1KHz
    T2CON=0b00000000;
    CCPR2L=1;
    CCP2CON=0b00001100;    
    CCPR1L=249;
    CCP1CON=0b00001100;    
    TMR2ON=1;
    /*TMR1=60536;
    T1CON=0b10000000;
    //CCPR1=60536+125;
    CCPR1=60536+625;
    CCP1CON=0b00001001;
    TMR1IF=0;
    TMR1IE=1;
    PEIE=1;
    GIE=1;
    TMR1ON=1;*/
    while(1){
      LATD0=1;
      __delay_ms(100);  //Duty=ton/T
      LATD0=0;
      __delay_ms(100);
      if(CCPR2L>248)
        CCPR2L=1;
      CCPR2L=CCPR2L+2;
      if(CCPR1L<2)
        CCPR1L=249;
      CCPR1L=CCPR1L-2;      
    }
}
void interrupt ISR(void){
    /*TMR1IF=0;
    TMR1=60536;
    //CCPR1=60536+125;
    CCPR1=60536+375;
    CCP1CON=0b00001001;*/    
}