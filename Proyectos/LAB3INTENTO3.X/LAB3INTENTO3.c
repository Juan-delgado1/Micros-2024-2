// El código no tiene acciones en el ciclo principal, funciona  por interrupciones principalmente

#include <xc.h>             // Busca librería en especifico no es necesario traer cada librería 
#define _XTAL_FREQ  4000000 // Hz configura el reloj

#pragma config FOSC=HS      // Clock Cristal
#pragma config WDT=OFF      // Perro Guardian
#pragma config LVP = OFF    // Desactiva Low Voltage Programming
#pragma config PBADEN = OFF // Desactiva Funciones Análogas

// VARIABLES GLOBALES
unsigned char contador;     // Contador 7 segmentos
unsigned char countD;       // Contador RGB



void __interrupt() ISR(void); // Esqueleto de las interrupciones

void main (void){       // Inicio de ejecucion

    
    // Definir roles de los pines: 0 salidas - 1 entrada
    // Puertos A y E para el 7 segmentos
    TRISA = 0x00;       // 0x00 significa que todos los pines del puerto A son salidas
    TRISE = 0x00;       
    TRISD = 0X00;       // RD2,RD3 y RD4 para RGB - RD7 Buzzer - 
    TRISB = 0xF7;       // 11110111: Entradas para interrupciones y RB3 para led Timer0
      
    // Estado Inicial de los pines
    // Cero en 7Seg
    LATA = 0xFF;        // LATA = 11111111
    LATE = 0x02;        // LATE = 00000010
    // Negro en RGB
    LATD = 0x00;     
    
    
    // HLVDCON: HIGH/LOW-VOLTAGE DETECT CONTROL REGISTER
    // Deteccion bajo voltaje - Detecta al bajar la el volatje de 3,9v
    HLVDEN = 1;     // Habilitacion deteccion bajo voltaje
    VDIRMAG = 0;    // 0 detecta cuando el voltaje baja - 1 detcta cuando sube el voltaje
    // HLDVL = 1010 - Limite voltaje 3.90 V
    HLVDL3 = 1;     
    HLVDL2 = 0;
    HLVDL1 = 1;
    HLVDL0 = 0;     // Hasta aqui el hlvdl
    HLVDIF = 0;     // Bandera de interrupcion
  
    
    // Registro T0CON: 0,5 segundos para LED de funcionamiento
    T0CON = 0b00000010;     // Primeros 3 bits '010': Prescaler 8 - Bit 6 '0': Timer 16 bits
    TMR0 = 3036;            // Precarga inicial
    // Formula: Precarga = 2^Nbits - (Tiempo * Frecuencia del bus) / preescaler
    // Reemplazando 3036 = 2^16 - (0,5 * 1000000) / 8   
    TMR0IF = 0;             // Bandera de la interrupcion del Timer0 - Inicia abajo
    TMR0IE = 1;             // Habilita interrupción de Timer 0
    
    
    //Interrupción INT0-RB0 para Parada de emergencia
    INTEDG0=1;      // Interrupción en flanco de subida      
    INT0IF =0;      // Bandera de interrupcion
    INT0IE=1;       // Habilitacion de interrupcion
    
    // Interrupcion INT1-RB1 para Reinicio de conteo
    INTEDG1=1;      // Interrupcion en flanco de subida 
    INT1IF =0;      // Bandera de interrupción
    INT1IE=1;       // Habilita la interrupción
    
    // Interrupción INT2-RB2 para pulsador de la cuenta
    INTEDG2=1;      // Interrupcion en flanco de subida         
    INT2IF =0;      // Bandera de interrupcion
    INT2IE=1;       // Habilita la interrupcion
    
    GIE=1;          // Bit de habilitación golbal de interrupciones
    TMR0ON = 1;     // Habilita Timer0 antes de iniciar el bucle de ejecución
  
    
    // Iniciar contadores en 0
    contador = 0;   
    countD = 0;
    
    
    // Deteccion de un reinicio por instrucción en el software
    if(RI == 0){
        LATD = 0x14;  //00011000: RGB en Magenta
        // 7seg en cero
        LATA = 0xFF;  // LATA = 11111111
        LATE = 0x02;  // LATB = 00000010
        RI = 1;       // Desactiva la bandera 
    }
    

    
    while (1) { 
        
        // No hay instrucciones que repita de manera cíclica
        // Las instrucciones se hacen por interrupciones
           
    }
}

// Interrupciones
void __interrupt() ISR(void){

    // Interrupción de Timer0
    if(TMR0IF == 1){
        
        if (HLVDIF) {   // Si se detecta un voltaje bajo
            HLVDIF = 0; // Limpiar la bandera de interrupción
            TMR0 = 49911;       // Precarga    
        } else {        //  Si no detecta voltaje bajo
            TMR0 = 3036;        // Precarga
        }
        
        TMR0IF = 0;         // Baja bandera
        LATB3 = LATB3^1;    // XOR: Cambio de valor del LED en RB3 de 0 a 1 ó de 1 a 0
        // El LED cambiará cada 0,5 segundos
        // Perido de 1 segundo, frecunecia de 1 Hz
    }
    
    // Interrupcion INT0 - RB0: Parada de emergencia
    if(INT0IF == 1){
        INT0IF = 0;             // Bajar bandera
        __delay_ms(50);         // Antirebote
        if(RB0 == 1){           // Verifica que el interruptor haya subido
            while(RB0 == 1 || RB0 == 0){
                // Despues de haber activado la interrupción se quedará en este ciclo
                // Para salir debe utilizar MCLR
                LATD = 0x10; //00010000: Rojo en RGB
            }
        }
    }
    
    //Interrupcion INT1 - RB1: Reinicio de conteo
    if(INT1IF == 1){
        INT1IF = 0;     // Desactivar bandera
        __delay_ms(50); // Antirebote
        if(RB1 == 0){
            // Si RB1 no es 1, nada ocurre
            NOP();
        } else {
            // Si RB1 es 1, se reinicia el conteo
            RESET();  
        }          
    }
    
    // Interrupción INT2 - RB2: Pulsador de conteo    
    if(INT2IF == 1){
        __delay_ms(50); // Antirebote
        if(RB2 == 1 && RB0 == 0){ 
            // Solo realizará el conteo si RB2 es 1
            // y si el interruptor de emergencia este desactivado RB2
            
            contador = contador + 1;    // Al pulsar aumentará el conteo del 7 seg
            
            // Verifica valor máximo del /seg
            if(contador > 9){           
                //Si el contador del 7seg es mayor a 9, si llega al máximo del 7 seg
                LATD7=1;                // Activa el buzzer
                __delay_ms(500);
                LATD7=0;                // Desactiva el buzzer en medio segundo (corto)
                contador = 0;           // Reinicia el contador
                countD = countD + 1;    // Aumenta el contador del RGB
            }
    
            // Verifica valor máximo del RGB
            if(countD == 6){
                //Si el contador del RGB es igual a 6, si llega al máximo del RGB
                LATD7=1;            // Activa el buzzer
                __delay_ms(1000);
                LATD7=0;            // Desactiva el buzzer en 1 segundo (largo)
                countD = 0;         // Reinicia el contador                
            }     
               
            
            // Después de verificar los valores máximos, se encienden los pines del RGB
            if(countD == 0 && contador == 0){ 
                //Valores iniciales de ambos contadores
                //magenta
                LATD = 0x14; //00010100
                 
            } else if (countD == 0){ 
                //magenta
                LATD = 0x14; //00010100
                
            } else if (countD == 1){ 
                //azul
                LATD = 0x04; //00000100
                    
            } else if (countD == 2){ 
                // cyan
                LATD = 0x0C; //00001100
                    
            } else if (countD == 3){
                //verde
                LATD = 0x08; //00001000
                    
            }  else if (countD == 4){
                // amarillo
                LATD = 0x18; //00011000
                    
            }  else if (countD == 5){
                //blanco
                LATD = 0x1C;  //00011100
         
            }
       
            // Ahora los pines del 7seg
            if(contador == 0){
                //0
                LATA = 0xFF;  // LATA = 11111111
                LATE = 0x02;  // LATB = 00000010

            } else if (contador == 1){
                //1
                LATA = 0x0D;  // LATA = 00001101
                LATE = 0x00;  // LATB = 00000000
  
            } else if (contador == 2){
                //2
                LATA = 0xFB;  // LATA = 11111011
                LATE = 0x01;  // LATB = 00000001

            } else if (contador == 3){
                //3
                LATA = 0xEF;  // LATA = 11101111
                LATE = 0x01;  // LATB = 00000001

            }  else if (contador == 4){
                //4
                LATA = 0xCD;  // LATA = 11001101
                LATE = 0x03;  // LATB = 00000011

            }  else if (contador == 5){
                //5
                LATA = 0xEE;  // LATA = 11101110
                LATE = 0x03;  // LATB = 00000011

            }  else if (contador == 6){
                //6
                LATA = 0xFE;  // LATA = 11111110
                LATE = 0x03;  // LATB = 00000011
 
            }  else if (contador == 7){
                //7
                LATA = 0xCF;  // LATA = 11001111
                LATE = 0x00;  // LATB = 00000000
  
            } else if (contador == 8){
                //8
                LATA = 0xFF;  // LATA = 11111111
                LATE = 0x03;  // LATB = 00000011
      
            } else if (contador == 9){
                //9
                LATA = 0xEF;  // LATA = 11101111
                LATE = 0x03;  // LATB = 00000011
    
            }         
        }
        
        INT2IF = 0;     // Bajar bandera
    }

          
}



