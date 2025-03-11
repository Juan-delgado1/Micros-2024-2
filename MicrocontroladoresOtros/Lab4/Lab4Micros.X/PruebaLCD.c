#include <xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC9.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma config PBADEN=OFF
#pragma config LVP=OFF

unsigned char Tecla=0;
int flagLuz = -1;
int flagEscribir = 0;
int flagDig = 0;
int flagInicio = 0;
int flagEmergencia =0;
int flagFin=0;
int flagCont=0;
int cuenta = 0;
int u;
int d;
int a;
int b;
int cuenObj;
char cuenMos;
int piezFalt;

void interrupt ISR(void);
void luzFondo(void);
int escribirDig(unsigned char);
void borrarTexto(void);
void ok();
void contador(void);
void RGB();
void SEG();
void parEmeg();
void reiCon();
void fin();

void main(void){
    ADCON1=15;
    TRISD=0;
    TRISA=0;
    TRISC1=1;
    TRISE0 = 0;
    TRISE1 = 0;
    TRISE2 = 0;
    LATD=0;
    TRISB=0b11110000;;
    RBPU=0;
    __delay_ms(100);
    TMR0=3036;
    T0CON=0b00000001;
    TMR0IF=0;
    TMR0IE=1;
    TMR0ON=1;
    RBIF=0;
    RBIE=1;
    GIE=1;
    
    //RGB EN BLANCO
    LATE0 = 0;
    LATE1 = 0;
    LATE2 = 0;
    
    //Inicializacion
    __delay_ms(1000);
    ConfiguraLCD(4);
    InicializaLCD();
    
    
    
    if(flagEmergencia == 0){
        //Primer mensaje
        MensajeLCD_Var("Bienvenidos");
        CaracterCara();
        EscribeLCD_c(1);
        CaracterQbert();
        EscribeLCD_c(2);
        __delay_ms(5000);
        BorraLCD();

        //Segundo Mensaje
        MensajeLCD_Var("Piezas a Contar: ");
        DireccionaLCD(0xC0);
        flagEscribir=1;
       while(1){
           LATA1=1;
           __delay_ms(125);
           contador();
           __delay_ms(125);
           contador();
           __delay_ms(125);
           contador();
           __delay_ms(125);
           LATA1=0;
           __delay_ms(125);
           contador();
           __delay_ms(125);
           contador();
           __delay_ms(125);
           contador();
           __delay_ms(125);
       } 
    }
}

void luzFondo(void){
       flagLuz*=-1;
       if(flagLuz==1)LATA3=1;
       else if(flagLuz==-1)LATA3=0;
}

int escribirDig(unsigned char lectura){
    if(flagEscribir == 1){
        if(flagDig==0){
            d=lectura;
            EscribeLCD_c(d+ '0');
            flagDig=1;
            return 0;
        }
        else if (flagDig==1){
            u=lectura;
            EscribeLCD_c(u+ '0');
            flagDig=2;
            return 0;
        }
    }
        
}

void ok(){
    if(flagEscribir == 1 && flagFin==0){
        cuenObj=(d*10)+u;
           
        if(cuenObj<60 && cuenObj>0){
            flagCont=1;
            piezFalt = cuenObj;
            flagEscribir=0;
            BorraLCD();
            //Tercer Mensaje
            MensajeLCD_Var("Faltan: ");
            EscribeLCD_n8(piezFalt,2);
            DireccionaLCD(0xC0);
            MensajeLCD_Var("Objetivo: ");
            EscribeLCD_n8(cuenObj,2);
            flagDig=0;
        }
        else{
            BorraLCD();
            MensajeLCD_Var("Valor Invalido");
            flagDig=0;
            __delay_ms(3000);
            borrarTexto();
            } 
        }
    else if(flagFin==1){
        //Segundo Mensaje
        BorraLCD();
        MensajeLCD_Var("Piezas a Contar: ");
        DireccionaLCD(0xC0);
        flagEscribir=1;
        flagDig=0;
        
        flagFin=0;
        piezFalt=cuenObj;
        cuenta=0;
        a=0;
        b=0;
        RGB();
        SEG();
    }
        contador();
    }

void borrarTexto(void){
    if(flagEscribir == 1){
        BorraLCD();
        MensajeLCD_Var("Piezas a Contar: ");
        DireccionaLCD(0xC0);
        flagEscribir=1;
        cuenObj=0;
        flagDig=0;
    }
}

void contador(void){
    if(flagCont==1){
        if(PORTCbits.RC1==0){
           flagInicio = 1;
           if(flagEmergencia == 0){
               cuenta+=1;
               piezFalt-=1;
               a = cuenta % 10;
               b = (cuenta - a)/10;
               RGB();
               SEG();
               //Tercer Mensaje
               BorraLCD();
               MensajeLCD_Var("Faltan: ");
               EscribeLCD_n8(piezFalt,2);
               DireccionaLCD(0xC0);
               MensajeLCD_Var("Objetivo: ");
               EscribeLCD_n8(cuenObj,2);
               if (cuenta==cuenObj){
                    BorraLCD();
                    MensajeLCD_Var("Fin de Cuenta");
                    DireccionaLCD(0xC0);
                    MensajeLCD_Var("Presione OK");
                    flagFin=1;

               }
           }
        }
    }
}

void RGB(){
    if (flagInicio == 0){
        LATE0 = 0;
        LATE1 = 0;
        LATE2 = 0;
    }
    else if (b == 0){
        LATE0 = 1;
        LATE1 = 0;
        LATE2 = 1;
    }
    else if (b == 1){
        LATE0 = 0;
        LATE1 = 0;
        LATE2 = 1;
    }
    else if (b == 2){
        LATE0 = 0;
        LATE1 = 1;
        LATE2 = 1;
    }
    else if (b == 3){
        LATE0 = 0;
        LATE1 = 1;
        LATE2 = 0;
    }
    else if (b == 4){
        LATE0 = 1;
        LATE1 = 1;
        LATE2 = 0;
    }
    else if (b == 5){
        LATE0 = 1;
        LATE1 = 1;
        LATE2 = 1;
    }
}

void SEG(){
    LATD = a;
}

void parEmerg(void){
    LATE0 = 1;
    LATE1 = 0;
    LATE2 = 0;
    flagEmergencia = 1;
    BorraLCD();
    MensajeLCD_Var("Emergencia");
}

void reiCon(void){
    piezFalt=cuenObj;
    cuenta=0;
    a=0;
    b=0;
    RGB();
    SEG();
    //Tercer Mensaje
    BorraLCD();
    MensajeLCD_Var("Faltan: ");
    EscribeLCD_n8(piezFalt,2);
    DireccionaLCD(0xC0);
    MensajeLCD_Var("Objetivo: ");
    EscribeLCD_n8(cuenObj,2);
}

void fin(void){
        cuenta=cuenObj;
        BorraLCD();
        MensajeLCD_Var("Fin de Cuenta");
        DireccionaLCD(0xC0);
        MensajeLCD_Var("Presione OK");
        flagFin=1;
        flagDig=0;
        flagCont=0;
}

void interrupt ISR(void){
    if(flagEmergencia==0){
        if(RBIF==1){
            if(PORTB!=0b11110000){
                Tecla=0;
                LATB=0b11111110;
                if(RB4==0){Tecla=1;escribirDig(1);}
                else if(RB5==0){Tecla=2;escribirDig(2);}
                else if(RB6==0){Tecla=3;escribirDig(3);}
                else if(RB7==0){Tecla=4;ok();}
                else{
                    LATB=0b11111101;
                    if(RB4==0) {Tecla=5;escribirDig(4);}
                    else if(RB5==0) {Tecla=6;escribirDig(5);}
                    else if(RB6==0) {Tecla=7;escribirDig(6);}
                    else if(RB7==0) {Tecla=8; parEmerg();}
                    else{
                        LATB=0b11111011;
                        if(RB4==0){Tecla=9;escribirDig(7);}
                        else if(RB5==0) {Tecla=10;escribirDig(8);}
                        else if(RB6==0) {Tecla=11;escribirDig(9);}
                        else if(RB7==0) {Tecla=3;borrarTexto();}
                        else{
                            LATB=0b11110111;
                            if(RB4==0) {Tecla=13;reiCon();}
                            else if(RB5==0){Tecla=14;escribirDig(0);}
                            else if(RB6==0) {Tecla=15;fin();}
                            else if(RB7==0) {Tecla=16;luzFondo();}
                        }
                    }
                }
            }

            LATB=0b11110000;
            __delay_ms(50);
            RBIF=0;
        }
        if(TMR0IF==1){
            TMR0IF=0;
            TMR0=3036;
            LATD1=LATD1;
        }
    }    
}