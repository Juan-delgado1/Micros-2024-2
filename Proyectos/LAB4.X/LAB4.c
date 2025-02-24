#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma config PBADEN=OFF
#pragma config LVP=OFF          //Desactivar RB5 bajo voltaje

unsigned char Tecla;
unsigned char *aux[]="Piezas a contar:";
unsigned char *OK[]="OK";
unsigned char *PARO[] = "PARO";
unsigned char *SUPR[] = "SUPR";
unsigned char *FIN[] = "FIN";
unsigned char objetivo;
unsigned char faltantes;
unsigned char contadas;
unsigned char input;
unsigned char ok;
void __interrupt() ISR(void); // Esqueleto de las interrupciones
void main(void){
    
    // Asignación de puertos
    ADCON1 = 15;        // Desactivar funciones analogas del puerto A
    TRISA = 0x00;       // A5:Enable pantalla=0 //A4:LuzdeFondo //A3:RS=0 //A2:Buzzer //A1:LuzdeCPU
    TRISB = 0b11110000; // Altas entradas 1 Columnas - Bajas salidas 0 Filas
    TRISC = 0xFF;       // C0 Sensor de barrera
    TRISD = 0x00;       // D0-3 7segmentos  -  D4-7 LCD
    TRISE = 0x00;       // RGB
    
    // Inicio de puertos
    LATA = 0b010000;
    LATB = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    RBPU=0;             // Activa resistencias de pullup
    
    // Interrupciones
    
    // Interrupción de teclado para todo el puerto B
    RBIF=0;             // Bandera puerto B
    RBIE=1;             // Enable de interrupción
   
    
    // Registro T0CON: 0,5 segundos para LED de funcionamiento
    T0CON = 0b00000010;     // Primeros 3 bits '010': Prescaler 8 - Bit 6 '0': Timer 16 bits
    TMR0 = 49911;            // Precarga inicial
    // Formula: Precarga = 2^Nbits - (Tiempo * Frecuencia del bus) / preescaler
    // Reemplazando 3036 = 2^16 - (0,5 * 250000) / 8   
    TMR0IF = 0;             // Bandera de la interrupcion del Timer0 - Inicia abajo
    TMR0IE = 1;             // Habilita interrupción de Timer 0
    
       
    GIE=1;          // Bit de habilitación golbal de interrupciones
    TMR0ON = 1;     // Habilita Timer0 antes de iniciar el bucle de ejecución
    
    
    // Configuración LCD
    input = 0;
    objetivo = 0;
    ok = 0;
    ConfiguraLCD(4);
    InicializaLCD();
    
    //Mensaje binevenida
    BorraLCD();
    MensajeLCD_Var("Bienvenida");
    __delay_ms(1000);
    
    // Piezas a contar
    BorraLCD();
    MensajeLCD_Var(*aux);
    DireccionaLCD(0xC0);
    __delay_ms(1000);
    
    //Mientras no presione OK no pase nada
    while(ok == 0){
        
    }
    
    // Definición variables
    objetivo = objetivo + Tecla;
    faltantes = objetivo;
    contadas = objetivo - faltantes;
    
    // Inicio conteo
    BorraLCD();
    MensajeLCD_Var("Faltantes :");
    EscribeLCD_n8(faltantes,2);
    DireccionaLCD(0xC0);
    MensajeLCD_Var("Objetivo :");
    EscribeLCD_n8(objetivo,2);
    
    while(1){
    if(RC0 == 1){
        contadas++;
        faltantes--;
        LATD = contadas;
        
            BorraLCD();
            MensajeLCD_Var("Faltantes :");
            EscribeLCD_n8(faltantes,2);
            DireccionaLCD(0xC0);
            MensajeLCD_Var("Objetivo :");
            EscribeLCD_n8(objetivo,2);
            
        
    }
        
    }  
}
    


void __interrupt() ISR(void){
    
    // Interrupción TIMER0
    if(TMR0IF == 1){ 

        TMR0 = 49911;       // Precarga       
        TMR0IF = 0;         // Baja bandera
        LATA1 = LATA1^1;    // XOR: Cambio de valor del LED en RB3 de 0 a 1 ó de 1 a 0
        // El LED cambiará cada 0,5 segundos
        // Perido de 1 segundo, frecunecia de 1 Hz
    }
    
    // Interrupción del Teclado
    if(RBIF==1){
        
        if(PORTB!=0b11110000){
            
            LATB=0b11111110;
            if(RB4==0){
                RESET();
            }
            else if(RB5==0){ 
                input++;
                if(input <= 2){
                    Tecla = 0;
                    EscribeLCD_n8(Tecla,1); 
                }
                
                
            }
            else if(RB6==0){               
                MensajeLCD_Var(*FIN);
            }
            else if(RB7==0){
                
                LATA4  = LATA4^1;
            }
            else{
                LATB=0b11111101;
                if(RB4==0) {
                    input++;
                    if(input <= 2){
                        Tecla = 7;
                        EscribeLCD_n8(Tecla,1);
                    }

                }
                else if(RB5==0){
                            input++;
                            if(input <=2){
                                Tecla = 8;
                                EscribeLCD_n8(Tecla,1); 
                            }
  
                        }
                else if(RB6==0){
                            input++;
                            if(input <=2){
                                Tecla = 9;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                else if(RB7==0) {                         

                  
                            MensajeLCD_Var(*SUPR);
                        }
                else{
                    LATB=0b11111011;
                    if(RB4==0){
                            input++;
                            if(input <=2){
                                Tecla = 4;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                    else if(RB5==0){
                            input++;
                            if(input <=2){
                                Tecla = 5;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                    else if(RB6==0){
                            input++;
                            if(input <=2){
                                Tecla = 6;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                    else if(RB7==0){
                        MensajeLCD_Var(*PARO);
                    }
                    else{
                        LATB=0b11110111;
                        if(RB4==0){
                            input++;
                            if(input <=2){
                                Tecla = 1;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                        else if(RB5==0){
                            input++;
                            if(input <=2){
                                Tecla = 2;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                        else if(RB6==0){
                            input++;
                            if(input <=2){
                                Tecla = 3;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                        else if(RB7==0){
                            
                            ok++;
                        }
                    }
                }
            }
            LATB=0b11110000;
            
        }
        
        if(ok == 0){
            if(input == 1){
                
                objetivo = Tecla*10;
                
            } 
        }
        __delay_ms(250);
        RBIF=0;

        

    }

}