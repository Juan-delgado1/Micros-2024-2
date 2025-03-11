#include <xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC9.h"
#pragma config FOSC=INTOSC_EC //Oscilador Interno
#pragma config WDT=OFF //Apagar Perro guardian
#pragma config PBADEN=OFF //RB4:RB0 como entradas digitales
#pragma config LVP=OFF //Liberar RB5 de que bajo voltaje

//Libreria envio de datos 
#include <stdio.h> 
#include <stdlib.h>

//DECLARACION DE VARIABLES
unsigned char Tecla=0;
int flagLuz = -1;
int flagEscribir = 0;
int flagDig = 0;
int flagInicio = 0;
int flagEmergencia =0;
int flagFin=0;
int flagCont=0;
int flagADC=0;
int cuenta = 0;
int u;
int d;
int a;
int b;
int cuenObj;
char cuenMos;
int piezFalt;
unsigned int adcVal;

//DECLARACION DE FUNCIONES
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
void ADC();
void Transmitir(unsigned int);
void RecibirComando(void);

/////////////////////////////////////////

void main(void){
    TRISD=0; //Puerto D como salida
    TRISA=0; //Puerto A como salida
    
    //Inicializar RGB
    LATE0 = 0;
    LATE1 = 0;
    LATE2 = 0;
    
    TRISA0=1; //Entrada Potenciometro
    TRISC2=0; // Salida Motor
    
    //Configuración del ADC
    ADCON1 = 0x0E;//AN0 (RA0) en analogo
    ADCON2 = 0xA9;//FOS/8 | 12 ciclos del TADc | Right Justified ( ADRESH(+SIG) - ADRESL(-SIG))
    ADCON0 = 0x01; //Activar ADC
    
    //Comunicacion Serial
    TXSTA=0b00100100;//Activar transmision, asincrono y alta velocidad
    RCSTA=0b10010000;//Activar serial y recepcion continua
    BAUDCON=0b00001000;// BAUD RATE 16-bits    
    SPBRG=25;  // SPBRG = ((Fosc)/(4 x Baudrate)) - 1 
    
    TRISC1=1; //C1 como entrada, contador
    
    //E0, E1, E2 SALIDAS RGB
    TRISE0 = 0;
    TRISE1 = 0;
    TRISE2 = 0;
    
    LATD=0; //Inicializa 7seg en 0
    
    TRISB=0b11110000; //Entradas - Salidas teclado matricial
    RBPU=0; //Res Pullup on
    __delay_ms(100); //Para res Pullup
    
    //Confic Interrupciones
    RBIF=0; //Limpia Variable de interrupcion
    RBIE=1; //Activar interrupcion puerto B
    GIE=1; //Activar interrupcion global
    
    //Inicializacion LCD
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
           ADC();
           RecibirComando();
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
    LATC2 = 0;
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
        a = cuenta % 10;
        b = (cuenta - a)/10;
        RGB();
        SEG();
        BorraLCD();
        MensajeLCD_Var("Fin de Cuenta");
        DireccionaLCD(0xC0);
        MensajeLCD_Var("Presione OK");
        flagFin=1;
        flagDig=0;
        flagCont=0;
}

void ADC (void){
    if(flagEmergencia==0){
        // Iniciar la conversión A/D
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO_nDONE);  // Esperar a que termine la conversión

        // Leer el resultado de la conversión (10 bits)
        adcVal = ((ADRESH << 8) + ADRESL); //ADRESH=+signif, ADRESL=-signif, << desplazamiento de 8bits 
        // Convertir a voltaje (0-1023 corresponde a 0-5V)
        float voltaje = (adcVal * 5.0) / 1023.0;

        // Controlar el LED según el voltaje leído
        if(flagADC==0){
            if (voltaje > 2.5) {
                LATC2 = 1;  // Encender LED
            } else {
                LATC2 = 0;  // Apagar LED
            }
        }

        Transmitir(adcVal);
    }
}

void Transmitir(unsigned int adcVal) {
    char buffer[50]; //arreglo 
    
    // Convierte el valor ADC a una cadena de caracteres con libreria
    sprintf(buffer, "Valor de ADC: %u\r", adcVal);
    
    // Transmite cada carácter del buffer
    for (int i = 0; buffer[i] != '\0'; i++) {
        while (TRMT == 0); // Espera a que el registro de transmisión esté vacío
        TXREG = buffer[i]; // Transmite el carácter
    }
}

void RecibirComando(void){
    if (RCIF){ // Si se ha recibido un dato
        char comando = RCREG; // Leer el dato recibido
        
        // Ejecutar la acción correspondiente según el comando recibido
        switch (comando){
            case 'P':
            case 'p':
                parEmerg();
                break;
            case 'E':
            case 'e':
                if(flagEmergencia==0){LATC2 = 1;flagADC=1;}
                break;
            case 'A':
            case 'a':
                if(flagEmergencia==0){LATC2 = 0;flagADC=1;}
                break;
            case 'R':
            case 'r':
                reiCon();
                flagADC=1;
                break;
            default:
                // Opcional: manejar comandos no reconocidos
                break;
        }
        
        // Limpiar la bandera de interrupción
        RCIF = 0;
    }
}

void interrupt ISR(void){
    if(flagEmergencia==0){
        if(RBIF==1){
            if(PORTB!=0b11110000){
                Tecla=0;
                LATB=0b11111110;
                if(RB4==0)escribirDig(1);
                else if(RB5==0)escribirDig(2);
                else if(RB6==0)escribirDig(3);
                else if(RB7==0)ok();
                else{
                    LATB=0b11111101;
                    if(RB4==0)escribirDig(4);
                    else if(RB5==0)escribirDig(5);
                    else if(RB6==0)escribirDig(6);
                    else if(RB7==0)parEmerg();
                    else{
                        LATB=0b11111011;
                        if(RB4==0)escribirDig(7);
                        else if(RB5==0)escribirDig(8);
                        else if(RB6==0)escribirDig(9);
                        else if(RB7==0)borrarTexto();
                        else{
                            LATB=0b11110111;
                            if(RB4==0)reiCon();
                            else if(RB5==0)escribirDig(0);
                            else if(RB6==0)fin();
                            else if(RB7==0)luzFondo();
                        }
                    }
                }
            }

            LATB=0b11110000;
            __delay_ms(50);
            RBIF=0;
            
        }
    }    
}