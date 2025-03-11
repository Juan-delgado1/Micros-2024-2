#include <xc.h>
//Libreria envio de datos 
#include <stdio.h> 
#include <stdlib.h>

#define _XTAL_FREQ 1000000
#include "LibLCDXC9.h"

#pragma config FOSC=INTOSC_EC //Oscilador Interno
#pragma config WDT=OFF //Apagar Perro guardian
#pragma config PBADEN=OFF //RB4:RB0 como entradas digitales
#pragma config LVP=OFF //Liberar RB5 de que bajo voltaje

#define TRIGGER RC0
#define ECHO RC1
////////////////////////////////////////////////////////////////////////////////

///DECLARACION DE VARIABLES///
unsigned char Tecla=0;
int flagLuz = -1;
int flagEscribir = 0;
int flagDig = 0;
int flagInicio = 0;
int flagEmergencia =0;
int flagFin=0;
int flagCont=0;
int flagADC=0;
int flagObjeto=0;
int cuenta = 0;
int u;
int d;
int a;
int b;
int cuenObj;
char cuenMos;
int piezFalt;
float adcVal;
float pwm=0;
unsigned char dis;
unsigned char etimeout=0;

///DECLARACION DE FUNCIONES///
void interrupt ISR(void);
void luzFondo(void);
int escribirDig(unsigned char);
void borrarTexto(void);
void ok();
void contador(unsigned char);
void RGB();
void SEG();
void parEmeg();
void reiCon();
void fin();
void ADC();
void Transmitir(float);
void RecibirComando(void);
unsigned char MedirDistancia(void);
void putch(char data);

////////////////////////////////////////////////////////////////////////////////

///MAIN///
void main(void){
    //Oscilador
    OSCCON=0b01000000;  // oscilador interno se ajusta a 1 MHz
    __delay_ms(1);      // se espera que se estabilice el oscilador interno
    
    TRISD=0; //Puerto D como salida
    TRISA=0; //Puerto A como salida
    
    //E0, E1, E2 SALIDAS RGB
    TRISE0 = 0;
    TRISE1 = 0;
    TRISE2 = 0;
    //Inicializar RGB
    LATE0 = 0;
    LATE1 = 0;
    LATE2 = 0;
    
    LATD=0; //Inicializa 7seg en 0
    
    TRISA0=1; //Entrada Potenciometro
    
    //Configuracion CCP
    TRISC2=0; // Salida Motor
    PR2=249; //Tpwm = (PR2 +1)*4*Tosc*Prescaler
    T2CON=0b00000000; //prescaler de 1 y se mantiene teporalmente apagado
    CCP1CON=0b00001100; //config en modo PWM
    TMR2=0;
    TMR2ON=1; 
    
    //Configuración del ADC
    ADCON1 = 0x0E;//AN0 (RA0) en analogo
    ADCON2 = 0xA9;//FOS/8 | 12 ciclos del TADc | Right Justified ( ADRESH(+SIG) - ADRESL(-SIG))
    ADCON0 = 0x01; //Activar ADC

    //Sensor HCSR04
    TRISC0=0;           // Se ajusta el pin de Trigger como salida
    TRISD0=0;           // Se ajusta una salida para un led de prueba
    LATC0=0;            // Se ajustan los valores iniciales de las salidas

    //Comunicacion Serial
    TXSTA=0b00100100;//Activar transmision, asincrono y alta velocidad
    RCSTA=0b10010000;//Activar serial y recepcion continua
    BAUDCON=0b00001000;// BAUD RATE 16-bits    
    SPBRG=25;  // SPBRG = ((Fosc)/(4 x Baudrate)) - 1 

    T1CON=0b10010000;   // Ajuste de timer1: prescaler 1 (no prescaler)
    
    //Teclado Matricial
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
           dis=MedirDistancia();   // Se realiza la medición de distancia
           if(dis>8 || dis < 5){flagObjeto = 0;}
           contador(dis);
           __delay_ms(250);
           dis=MedirDistancia();    // Se realiza la medición de distancia
           contador(dis);
           __delay_ms(250);
           LATA1=0;
           dis=MedirDistancia();    // Se realiza la medición de distancia
           contador(dis);
           __delay_ms(250);
           dis=MedirDistancia();   // Se realiza la medición de distancia
           __delay_ms(250);
           contador(dis);
       } 
    } 
}

///FUNCIONES DE TECLADO///
void luzFondo(void){
       flagLuz*=-1;
       if(flagLuz==1)LATA4=1;
       else if(flagLuz==-1)LATA4=0;
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
            flagADC=0; 
            dis=MedirDistancia();
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
        contador(dis);
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

void parEmerg(void){
    LATE0 = 1;
    LATE1 = 0;
    LATE2 = 0;
    LATC2 = 0;
    CCPR1L=0;
    flagADC=1; 
    pwm=0;
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
        __delay_ms(3000);
        CCPR1L=0;
        flagADC=1; 
        pwm=0;
        flagFin=1;
        flagDig=0;
}

///FUNCION DE CONTEO///
void contador(unsigned char dis){
    if(flagCont==1){
        if(dis <= 6 && dis >= 1 && flagObjeto == 0){
           flagInicio = 1;
           flagObjeto = 1;
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
                    flagCont=0;
                    fin();
               }
           }
        }
    }
}

///FUNCIONES DE SALIDAS///
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

///FUNCION DE MANEJO DE ADC///
void ADC (void){
    if(flagEmergencia==0){
        // Iniciar la conversión A/D
        ADCON0bits.GO = 1;
        while (ADCON0bits.GO_nDONE);  // Esperar a que termine la conversión

        // Leer el resultado de la conversión (10 bits)
        adcVal = ((ADRESH << 8) + ADRESL);
        
        // Controlar el motor según el voltaje leído
        if (flagADC == 0) {
            // Calcular pwm en rango de 0 a 1
            pwm = (float)adcVal / 1023.0;
            if(pwm == 1)CCPR1L = 249;
            else{CCPR1L = (unsigned char)(pwm * 250.0);}
        }
        Transmitir(pwm * 100);
    }
}

///FUNCIONES DE COMUNICACION SERIAL///
void Transmitir(float pwm){
    printf("Valor de PWM: %.2f %%\r",pwm);
    if(dis==0)              // Si la medida es 0 es por error del sensor
      printf("Falla en el sensor\r");
    else                  // Si no hay error se transmite la distancia
      printf("Distancia objeto: %d cm\r",dis);
}

void RecibirComando(void){
    if (RCIF){ // Si se ha recibido un dato
        char comando = RCREG; // Leer el dato recibido
        
        // Ejecutar la acción correspondiente según el comando recibido
        switch (comando){
            case 'P':
            case 'p':
                parEmerg();CCPR1L=0; flagADC=1; pwm=0;
                break;
            case 'N':
            case 'n':
                if(flagEmergencia==0){CCPR1L=249; flagADC=1; pwm=1;}
                break;
            case 'B':
            case 'b':
                if(flagEmergencia==0){CCPR1L=0.8*250; flagADC=1; pwm=0.8;}
                break;
            case 'V':
            case 'v':
                if(flagEmergencia==0){CCPR1L=0.6*250; flagADC=1 ;pwm=0.6;}
                break;
            case 'C':
            case 'c':
                if(flagEmergencia==0){CCPR1L=0.4*250; flagADC=1; pwm=0.4;}
                break;
            case 'X':
            case 'x':
                if(flagEmergencia==0){CCPR1L=0.2*250; flagADC=1; pwm=0.2;}
                break;
            case 'Z':
            case 'z':
                if(flagEmergencia==0){CCPR1L=0; flagADC=1; pwm=0;}
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

void putch(char data) {//Necesaria cuando se usa "printf()"
    while(TRMT == 0);  //Espera hasta que el registro de transmisión esté vacío
    TXREG = data;      //Carga el carácter a transmitir en el registro de transmisión
}

///FUNCION DE MEDIR DISTANCIA CON SENSOR ULTRASONICO///
unsigned char MedirDistancia(void){
  unsigned char aux=0;
  CCP2CON=0b00000100; // Ajustar CCP en modo captura con flanco de bajada
  TMR1=0;             // Iniciamos el timer1 en 0
  CCP2IF=0;           // Iniciar bandera CCPx en 0
  TRIGGER=1;          // Dar inicio al sensor
  __delay_us(10);
  TRIGGER=0;
  etimeout=1;         // Se habilita la condición de antibloqueo
  while(ECHO==0  && etimeout==1); // Se espera que el sensor empiece la
                      // medición o que pase el antibloqueo (aprox 2s)
  if(etimeout==0){    // Si el sensor no responde se retorna un 0
    return 0;
  }    
  TMR1ON=1;           // Se da inicio al timer1 o medición de tiempo
  while(CCP2IF==0 && TMR1IF==0);   // Espera a que la señal de ultrasonido regrese
  TMR1ON=0;           // Se da parada al timer 1 o medición de tiempo
  if(TMR1IF==1)       // Se comprueba que la medición del pulso del sensor no
    aux=255;          // exceda el rango del timer1, si es así se limita a 255
  else{  
    if((CCPR2*8)>=14732)  // Si el sensor excede 254cm se limita a este valor
      CCPR2=14732/8;
    aux=(CCPR2*8)/58 + 1; // Se calcula el valor de distancia a partir del tiempo
  }
  
  /*Se multipliza x 8 por el cambio de la frecuencia (8MHz->1MHz) */
  
  return aux;         // Se retorna la medición de distancia obtenida
}

///FUNCION DE INTERRUPCION (TECLADO)///
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
