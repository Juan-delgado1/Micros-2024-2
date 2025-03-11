#include<xc.h>
#define _XTAL_FREQ 1000000      // Frecuencia 1 MHz
#include "LibLCDXC8.h"          // Libreria LCD

#pragma config FOSC=INTOSC_EC   // Oscilador Interno
#pragma config WDT=OFF          // Desactiva Perro Guardian
#pragma config PBADEN=OFF       // Desactiva funciones analogicas
#pragma config LVP=OFF          // Desactivar Low Voltage Programming
#define TRIGGER RC0
#define ECHO RC1

unsigned char Tecla;            // Almacena el valor presionado en el teclado
unsigned char *aux[]="Piezas a contar:";        // 2do Mensaje para la LCD
unsigned char *FIN[] = "  COMPLETAS!!! ";       // Ultimo Mensaje LCD
unsigned char objetivo;         // ALmacena el numero de piezas que se quieren contar
unsigned char faltantes;        // Numero de piezas que falan por contar
unsigned char contadas;         // Numero de piezas contadas
unsigned char input;            // Almacena cuantos numeros se ha escrito y se muestran en la LCD
unsigned char block;            // Bloquea la tecla de suprimir y los numeros para escribir despues de dar OK
unsigned char ok;               // Variable que verifica si se presiono la tecla OK
unsigned char Iniciarpregunta;  // Habilita el teclado despues de la bienvenida
unsigned char emergency;        // Avisa si se activo el modo de emergencia
unsigned char contador;         // Cuenta los segundos de inactividad del PIC
unsigned char inactive;         //Es uno si se deja inactiva por 10 segundos
unsigned char etimeout=0,ctimeout=0;
unsigned char objeto = 0;
        unsigned char motorActual;
        unsigned char motorAntiguo;
        unsigned char Actual_sensor = 0; // Lee el estado actual del pin 4
        unsigned char flagMotor = 0;
        unsigned char valADC;
        unsigned char realPWM;
        unsigned char dis; 
         int valPWM;

const unsigned char banda1[] = {
    0b00000,
    0b01110,
    0b01010,
    0b01110,
    0b00000,
    0b11111,
    0b10101,
    0b11111
};

const unsigned char banda2[] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b10101,    
    0b11111
};


void Bienvenida(void);
void MostrarNumeros(void);      // Esqueleto de Funcion para mostrar un numero en el 7segmentos y el RGB
void Emergencia(void);          // Esqueleto de Funcion para activar el modo de parada de emergencia
void __interrupt() ISR(void);   // Esqueleto de las interrupciones
unsigned char Recibir(void);
void Transmitir(unsigned char BufferT);
void TransmitirVar(unsigned char *buffer);
unsigned char ConversionADC(unsigned char canal);
void numASCII(int number, unsigned char ascii_values[]);
unsigned char MedirDistancia(void);
void EEPROM_Write(unsigned char address, unsigned char data);
unsigned char EEPROM_Read(unsigned char address);

void main(void){
    
    
    
    // Asignaci�n de puertos
    TRISA = 0x01;       // A5:Enable pantalla=0 //A4:LuzdeFondo //A3:RS=0 
                        // A2:Buzzer //A1:LuzdeCPU //A0: Sensor de barrera
    TRISB = 0b11110000; // Entradas 1 Columnas - Salidas 0 Filas
    TRISC0 = 0;
    TRISC2=0;
    
    TRISD = 0x00;       // D0-3 7segmentos  -  D4-7 LCD
    TRISE = 0x00;       // RGB
    
    // Inicio de puertos
    LATA = 0b010000;    // Todos en 0 menos el de la luz de fondo
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    RBPU=0;             // Activa resistencias de pullup para el teclado
    
    
    // Configuracin RS232
    TXSTA = 0b00100100;    // TXEN = 1, BRGH = 1, SYNC = 0
    RCSTA = 0b10010000;    // SPEN = 1, CREN = 1 (Habilita recepci�n continua)
    BAUDCON = 0b00001000;  // BRGH16 = 1
    SPBRG = 25;            // 9600 bps con Fosc = 1MHz   
    RCIE = 1;
    
    // Config ADC
    ADCON0=0b00000001; // canal AN0 - RA0   y ADON = 1,
    ADCON1=0b00001110;
    ADCON2=0b00001000;
    
    // Config PWM motores
    TMR2=0;    
    PR2=249;    //Fpwm=1KHz Tpwm = 1ms
    T2CON=0b00000000;    
    CCPR1L=1;     //ton=(249+1)*d
    CCP1CON=0b00001100; 
    TMR2ON=1;
    
    
    //Configuracion Sensor
     T1CON=0b10000000;   //Ajuste de timer1: prescaler 1
    
    // Interrupciones
    
    // Interrupci�n de teclado para todo el puerto B
    RBIF=0;             // Bandera puerto B
    RBIE=1;             // Enable de interrupci�n
       
    // Registro T0CON: 0,5 segundos para LED de funcionamiento
    T0CON = 0b00000010;     // Primeros 3 bits '010': Prescaler 8  
                            // Bit 6 en '0': Timer 16 bits
    TMR0 = 49911;            // Precarga inicial
    // Formula: Precarga = 2^Nbits - (Tiempo * Frecuencia del bus) / preescaler
    // Reemplazando 49911 = 2^16 - (0,5 * 250000) / 8   
    TMR0IF = 0;             // Bandera de la interrupcion del Timer0 - Inicia abajo
    TMR0IE = 1;             // Habilita interrupci�n de Timer 0
          
    GIE=1;          // Bit de habilitaci�n golbal de interrupciones
    TMR0ON = 1;     // Habilita Timer0 antes de iniciar el bucle de ejecuci�n
    
    
    // Configuraci�n LCD
    ConfiguraLCD(4);
    InicializaLCD();
    OcultarCursorLCD();     // Funcion para no mostrar el cursor, agregado a la libreria
    

    


    // Detecta si No se reseteo por software
    if(RI == 1){ 
        // Si No se reseteo por software muestra el mensaje de bienvenida
        // Si se reseteo por softwre no muestra la bienvenida
        //Mensaje binevenida
        BorraLCD();                     // Borra todo lo que esta en la LCD
        Bienvenida();
        contador = 0;
        BorraLCD();
        MensajeLCD_Var("Bienvenida");   // Muestra el mensaje de bienvenida
        
        __delay_ms(1000);                // Muestra el mensaje hasta copletar 5 segundos
    } 
    
   // Detecta si se reseteo por software, si el numero ingresado no es v�lido
    if(RI == 0){
        // Pone el RGB en Magneta para que no inicie en negro
        LATE = 0b101;           // RGB
        RI = 1;                 // Desactiva bandera de reinicio por software
    }

    


    //CICLO PIRNCIPAL DEL C�DIGO
    // DIVIDIO EN 3: eleccion de piezas por contar, el conteo y la finalizaci�n del conteo
    while(1){
        
        // Inicio de variables en 0
        input = 0;      // cuantos numeros se han escrito = 0      
        objetivo = 0;   // Objetivo de piezas en este momento = 0
        ok = 0;         // no se ha presionado ok
        contador = 0;   // desde aqui empieza a ver la inactividad
        
        
        
        // ******   PRIMERA PARTE   ******
        // ******  Piezas a contar  ******
        BorraLCD();
        MostrarCursorLCD();         // Vuelve a activar el cursor para escribir
        MensajeLCD_Var(*aux);       // SEGUNDO MENSAJE
                                    //Pregunta numero de piezas en la l�nea de arriba
        DireccionaLCD(0xC0);        // Salta a la l�nea de abajo
        Iniciarpregunta=1;          // Habilita e teclado
        block = 0;                  // Habilita escritura de numeros y suprimir
        //__delay_ms(1000);          
    
        // ciclo para esperar que se ingresen los n�meros
        // se rompe al presionar OK
        while(ok == 0){
            if(emergency == 1)  Emergencia();      
            // Actia el modo de emergencia si se presiona la tecla
            // Se tuvo que hacer asi por que al realizarlo desde la interrupcion la LCD no mostraba los datos bien
        }
        
        //Despues de haber presionado OK ya se tiene el numero de piezas objetivo
        // VERIFICACION DE NUMERO INGRESADO
        // Se verifica si se escribio un numero
        if(input < 2){
            // si solo se escribi� un numero input = 1:
            Tecla = 0;                  // Se pone la tecla en 0
            objetivo = objetivo/10;     // Se ajusta el valor del objetivo
        }
        input = 3;            // Se pone input en 3 para esoecificar que ya se escribieron todos los numeros necesarios

        // Si el n�mero es mayor al rango se reinicia y va al segundo mensaje
        if(objetivo > 59){
            BorraLCD();                             // Primero Borra la LCD
            MensajeLCD_Var("Valor no valido");      // Muestra el mensaje de NO valido
            __delay_ms(2000);                       // 2seg para observar el mensaje
            RESET();                                // Reset por software
        }
    
        // Definici�n variables de piezas
        objetivo = objetivo + Tecla;        // Ajuste de objetivo
        faltantes = objetivo;               // Al inicio las piezas faltantes son todas
        contadas = objetivo - faltantes;    // las contadas van a ser las totales menos las que faltan
        
 
        
        // ******   SEGUNDA PARTE   ******
        // ******   INICIO CONTEO   ******
        block = 1;                      // Bloquea el suprimir y la escritura de numeros
        BorraLCD();                     // Borra la LCD
        MensajeLCD_Var("Faltantes :");  // Mensaje en Linea de arriba LCD
        EscribeLCD_n8(faltantes,2);     // Escribe el numero de piezas que faltan utilizando 2 digitos
        DireccionaLCD(0xC0);            // Direcciona la LCD a la primera casilla de la linea de abajo
        MensajeLCD_Var("Objetivo :");   // Mensaje en Linea de abajo LCD
        EscribeLCD_n8(objetivo,2);      // Escribe el numero de piezas ya contadas que faltan utilizando 2 digitos

        unsigned char Antiguo_sensor = 0;  // Variable pra observar el falnco de subida en el sensor de barrera pin 4
        
        // Ciclo de conteo y mostrar cambios del conteo
        while(1){
            if(emergency == 1){ // Primero verifica si hay emergencia
                Emergencia();
            }
            
            //unsigned char Actual_RC0 = RC0; // Lee el estado actual del pin 4
            
            // Deteccion sensor de barrera
            
            objeto = dis;
           
            //__delay_ms(150);
            
            if(objeto < 7 && objeto >= 1){
                Actual_sensor= objeto; // Lee el estado actual del pin 4
            } else {
                Actual_sensor = 0;
            }
            if( Actual_sensor != 0 && Antiguo_sensor == 0 && contadas < objetivo){ //Se�al del sensor
                // Si el sensor estaba en cero en el ciclo anterior y ahora esta en 1: hubo un fanco de subida
                // Se detecto una nueva pieza
                // Verifica que aun no se hayan contado todas las piezas
                
                contador = 0;       // Si hay actividad en el sensor, se pone en cero el contador de segundos de inactividad

                __delay_ms(100);    // Antirebote

                contadas++;         // Aumenta el numero de piezas contadas
                faltantes--;        // Disminuye el numero de piezas por contar

                if(contadas%10 == 0 && contadas != objetivo){
                    // Si se contaron 10 o un multiplo de 10
                    // suena el buzzer por medio segundo
                    LATA2 = 1;
                    __delay_ms(500);
                    LATA2 = 0;
                }
                MostrarNumeros();
            } 
            // Fin deteccion sensor de barrera
            
            Antiguo_sensor = Actual_sensor;   // El estado antiguo sera el que tiene ahora     

            //MostrarNumeros();           // Mostrar SOLO en el RGB y 7Seg el numero de piezas contadas 

            //LCD
            OcultarCursorLCD();         // Oculta el cursor de la LCD, funcion en la libreria
            DireccionaLCD(0x8B);        
            // Direcciona la LCD para que SOLO cambie el numero piezas faltantes y nada mas
            // Si recibe la se�al del sensor solo cambia el numero de piezas faltantes
            EscribeLCD_n8(faltantes,2); 
            // Escribe el numero de 8 bits con 2 digitos en el espacio de piezas faltantes

            
            // AL llegar al objetivo
            if (contadas == objetivo) {  
                LATA2 = 1;              // Se activa el buzzer
                BorraLCD();             // Borra toda la LCD
                MensajeLCD_Var(*FIN);   // Muestra mensjae de finalizaci�n
                __delay_ms(1000);       // Delay para que el buzzer suene 1 segundo
                LATA2 = 0;
                break;                  // Se sale del ciclo de conteo
            }
        }   // Fin ciclo conteo
    
        
        
        // ******   TERCERA PARTE   ******
        // ******  VUELTA A INICIO  ******
        // Se esta mostrando el mensaje de que termin�
        ok = 0;                 // Se pone la variable de ok en cero para volverla adetectar
        
        // Ciclo de espera: para realizar otra cuenta
        while(1){               
            if(emergency == 1){ // Verifica si se debe activar la emergencia
                Emergencia();
            }
            
            if(ok > 0){         // Cuando por fin se presione OK
                ok = 0;         // Variable Ok vuelve a 0
                objetivo = 0;   // objetivo de piezas vuelve a cero
                contadas = 0;
                input =0;       // se pone que no se ha escrito ningun numero
                MostrarNumeros();
                break;          // Se sale del ciclo de espera
                
            } // Fin ciclo de espera
            
            // Al terminar los 3 ciclos, se repite el ciclo general
            // Va al segundo mensaje a preguntar cuantas piezas a contar hay
        }
    
    }  // FIN ciclo general
 
} // FIN MAIN
    

// Interrupciones
void __interrupt() ISR(void){
    
    // Interrupci�n TIMER0
    if(TMR0IF == 1){ 

        TMR0 = 49911;       // Precarga       
        TMR0IF = 0;         // Baja bandera
        LATA1 = LATA1^1;    // XOR: Cambio de valor del LED en RB3 de 0 a 1 � de 1 a 0
        // El LED cambiar� cada 0,5 segundos
        // Perido de 1 segundo, frecunecia de 1 Hz
        
        //comunicacion serial

        if(LATA1 == 1){
           
            //Leer ADC
            
            if(flagMotor == 0){
                valADC = ConversionADC(0);
                valPWM = (valADC * 100/255); 
             } else {
                valPWM = (realPWM*100/240);
                
             }

            
            //Conversi�n
                     
            unsigned char numAscii[];
            numASCII(valPWM, numAscii);
            
            //Mostrar en pantalla
            TransmitirVar("Valor de PWM: ");            
            TransmitirVar(numAscii);
            Transmitir('%');
            TransmitirVar("\rDistancia objeto: ");
            dis = MedirDistancia();
            
            if(dis >= 10){
                dis = 9;
            }
            Transmitir(dis+48);
            Transmitir('\r');
            
            
            //Modificar PWM
            float d = 240*valPWM/100;
            CCPR1L=d;
                 
            
            
        }
        
        
     
    }
    
    // INTERRUPCIONO DE TECLADO
    if(RBIF==1 ){
        
        if(PORTB!=0b11110000 && Iniciarpregunta == 1){
            
            // verificacion pin accionado
            LATB=0b11111110; 
            // FILA DE MAS ABAJO
            
            if(RB4==0) { // 1ra Columna
                // Reinicio conteo
                faltantes = objetivo; // las faltantes vuelven a ser todas
                contadas = objetivo - faltantes; // se ajustan las contadas
                MostrarNumeros();
            }            
            else if(RB5==0){ // 2da columna: numero 0
                input++;        // Aumenta input avisando que se presiono otra tecla
                if(input <= 2){ // Si aun no se han presionado dos teclas
                    Tecla = 0;  // Guarda el valor en la variable Tecla
                    EscribeLCD_n8(Tecla,1); // Escribe el valor en la LCD
                }   
                // Este mismo funcionamiento tienen las demas teclas que corresponden a numeros
            }
            else if(RB6==0){   //3ra columna 
                // Forzar finalizaci�n del conteo            
                contadas = objetivo;    // se asume que ya secontaron todas sin pasar por el sensor
                faltantes = 0;          // no falta ninguna por contar
                MostrarNumeros();       // se muestra el numero toal de piezas en el rgb yel 7seg
                                        // El mensaje en la LCD ya se encarga el Ciclo de conteo                
            }
            else if(RB7==0){   //4ta columna
                // Activa o desactiva Luz de fondo  de la LCD           
                LATA4  = LATA4^1;
            }
            else{
                LATB=0b11111101;
                // Fila Media-ABAJO
                
                if(RB4==0) { // 1ra columna: numero 7
                    input++;
                    if(input <= 2){
                        Tecla = 7;
                        EscribeLCD_n8(Tecla,1);
                    } 
                }
                else if(RB5==0){ // 2da coolumna: numero 8
                    input++;
                    if(input <=2){
                        Tecla = 8;
                        EscribeLCD_n8(Tecla,1); 
                    }
                }
                else if(RB6==0){ // 3ra columna: numero 9
                    input++;
                    if(input <=2){
                        Tecla = 9;
                        EscribeLCD_n8(Tecla,1); 
                    }
                }
                else if(RB7==0) { // 4ta columna
                    // Eliminar �ltimo caracter ingresado
                    // Primero detecta cuantos numeros se han escrito 
                    // 1 o 2
                    if(input == 1){ // Si solo se ha escrito un numero
                        DireccionaLCD(0xC0); // Va la a primera casilla de abajo
                                             // que es donde esta el numero y
                        EscribeLCD_c(' ');   // lo reemplaza por un espacio
                        DireccionaLCD(0xC0); // vuelva a la primera casilla de abajo
                        input --;            // resta una unidad a la variable input 
                                             // para especificar que se borro un numero
                    } else if (input == 2){ // si ya se escribieron 2 numeros
                        DireccionaLCD(0xC1); //direcciona la lcd a la segunda casilla de abajo
                                             // que es donde esta el numero y
                        EscribeLCD_c(' ');   // reemplaza el numero por un 0
                        DireccionaLCD(0xC1); // vuelve a direccionar a la lcd a la segunda casilla
                        Tecla = objetivo/10;
                        input--;             // resta una unidad a la variable input 
                                             // para especificar que se borro un numer0
                    }else{
                        NOP(); // si no se ha escrito nada o el input se configuro mayor
                               // no hace nada y sale de la interrupcion
                    }
                    
                }
                else{
                    LATB=0b11111011;
                    // FILA Media-ALTA
                    
                    if(RB4==0){ // 1ra columna: numero 4
                        input++;
                        if(input <=2){
                            Tecla = 4;
                            EscribeLCD_n8(Tecla,1); 
                        }
                    }
                    else if(RB5==0){ // 2da columna: nuemro 5
                        input++;
                        if(input <=2){
                            Tecla = 5;
                            EscribeLCD_n8(Tecla,1); 
                        }
                    }
                    else if(RB6==0){    //3ra columna: numero 6
                        input++;
                        if(input <=2){
                            Tecla = 6;
                            EscribeLCD_n8(Tecla,1); 
                        } 
                    }
                    else if(RB7==0){ // columna numero 4
                        // Avisa que hay una emergencia
                        emergency = 1;                        
                    }
                    else{
                        LATB=0b11110111;
                        //FILA DE MAS ARRIBA
                        
                        if(RB4==0){ // 1ra columna; numero 1                            
                            input++;
                            if(input <=2){
                                Tecla = 1;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                        else if(RB5==0){ //2da columna numero 2
                            input++;
                            if(input <=2){
                                Tecla = 2;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                        else if(RB6==0){ //3ra columan numero 3
                            input++;
                            if(input <=2){
                                Tecla = 3;
                                EscribeLCD_n8(Tecla,1); 
                            }
                        }
                        else if(RB7==0){ //4ta columna
                            // avisa que se presiono OK, aumentando la variable                            
                            ok++;
                        }
                    }
                }
            }
            LATB=0b11110000;           
            // fin verificacion pines
            
        }       

        // Antes de salir de la interrupcion
        
        if(ok == 0){            //Si aun no se ha presionado OK y
            if(input == 1){     // Es el primer numero que se escribe           
                objetivo = Tecla*10;   // Toma ese numero como decenas, si solo se escribe un numero
                                       // En el while pirncipal se corrige
            } 
        }
        
        if(input > 2 && block == 0){ 
            // si aun esta escribiendo los numeros
            input =2; // input no puede pasar de 2 para que el boton de surpimimr pueda funcionar
        }
        
        __delay_ms(250); // antirebote
        contador = 0;    // al activar un boton del teclado
                         // el contador de inactividad vuelve a 0
        RBIF=0;          // la bandera se baja   

    }
    
    
    if (RCIF == 1){ // Si se ha recibido un dato
        RCIF = 0;
        char comando = RCREG; // Leer el dato recibido
        
        // Ejecutar la acci�n correspondiente seg�n el comando recibido
        switch (comando){
            case 'Z':
            case 'z':
                realPWM=0; 
                break;
            case 'X':
            case 'x':
                realPWM=240*0.2;
                break;
            case 'C':
            case 'c':
                realPWM=240*0.4;
                break;
            case 'V':
            case 'v':
                realPWM=240*0.6;
                break;
            case 'B':
            case 'b':
                realPWM=240*0.8;
                break;
            case 'N':
            case 'n':
                realPWM=240;
                break;
           
            default:
                // Opcional: manejar comandos no reconocidos
                break;
        }
        
        // Limpiar la bandera de interrupci�n
        
        CCPR1L = realPWM;
        flagMotor = 1;
        CREN = 1; 
        RCIE = 1;
        
    }
    

}

// Funcion para mostrar SOLO valores en el 7seg y el RGB
void MostrarNumeros(void){ 
    
    // 7segmentos solo muestra las unidades
    LATD = contadas % 10; // mostrara el residuo de dividir por 10 osea las unidades

    //RGB: RE0=ROJO RE1=VERDE RE2=AZUL
    if(contadas<10){
        //Magneta
        LATE = 0b101;
    } else if(contadas<20){
        // Azul
        LATE=0b100;
    } else if (contadas<30){
        // Cyan
        LATE=0b110;
    } else if (contadas<40){
        //Verde
        LATE=0b010;
    } else if (contadas<50){
        //Amarillo
        LATE = 0b011;
    } else if (contadas<60){
        //balnco
        LATE = 0b111;
    }
    
    EEPROM_Write(0x01, objetivo);
    EEPROM_Write(0x02, contadas);
    EEPROM_Write(0x03, faltantes);

}

// Funcion de parada de emergencia
void Emergencia(void){
    LATE = 0b001; // RGB en ROJO
    LATD = 8;     // 7segmentos con todos los leds prendidos
    BorraLCD();   // Borra la LCD para mostrar mensaje
    MensajeLCD_Var("    EMERGENCY");    //linea de arriba
    DireccionaLCD(0xC0);    // direccciona a la primera casilla de la linea de abajo
    MensajeLCD_Var("      STOP");   // linea de abajo
    CCPR1L=1;
    
    while(1){
        GIE = 0;    // se deshabilitan las interrupciones
                    // bucle infinito hasta reset por MCLR
    }
}


void Bienvenida(void){
    unsigned char pos = 0;  
    unsigned char max_pos = 15;  
    ConfiguraLCD(4);
    InicializaLCD();
    OcultarCursorLCD();
    CrearCaracter(0, banda1);
    CrearCaracter(1, banda2);
    for (pos = 0; pos <= max_pos; pos++) {

        DireccionaLCD(0x80 + pos);  
        EscribeLCD_c(1);  // Muestra la bolita normal
        __delay_ms(50);
    }

    for (pos = 0; pos < max_pos; pos++) {

        DireccionaLCD(0x80 + pos);  
        EscribeLCD_c(0);  // Muestra la bolita normal
        __delay_ms(50);
        DireccionaLCD(0x80 + pos); 
        EscribeLCD_c(1);
        __delay_ms(50);
    }

    DireccionaLCD(0x80 + max_pos);
    EscribeLCD_c(0); 
    for (pos = max_pos; pos > 0; pos--) {

        DireccionaLCD(0x80 + pos);  
        EscribeLCD_c(0);  
        __delay_ms(50);
        DireccionaLCD(0x80 + pos); 
        EscribeLCD_c(1);
        __delay_ms(50);
    } 
    BorraLCD;

    MostrarCursorLCD();      
}

unsigned char Recibir(void){
    while(RCIF == 0);  // Esperar a recibir un dato
    return RCREG;
}

void Transmitir(unsigned char BufferT){
    while(TRMT == 0);  // Esperar a que se vac�e el buffer de transmisi�n
    TXREG = BufferT;        // Transmitir en ASCII
}

void TransmitirVar(unsigned char *buffer){
    while (*buffer) {
        Transmitir(*buffer);
        buffer++;
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


void numASCII(int number, unsigned char ascii_values[]) {
    if (number < 10) {
        ascii_values[0] = '0' + number; // Solo un d�gito
        ascii_values[1] = '\0';        // Terminador nulo
    } else if (number < 100) {
        ascii_values[0] = '0' + number / 10; // Decenas
        ascii_values[1] = '0' + number % 10; // Unidades
        ascii_values[2] = '\0';              // Terminador nulo
    } else {
        ascii_values[0] = '1';
        ascii_values[1] = '0'; 
        ascii_values[2] = '0';              
        ascii_values[3] = '\0';              // Terminador nulo
    }
}

unsigned char MedirDistancia(void){
  unsigned char aux=0;
  CCP2CON=0b00000100; //Ajustar CCP en modo captura con flanco de bajada
  TMR1=0;             //Iniciamos el timer1 en 0
  CCP2IF=0;           //Iniciar bandera CCPx en 0
  TRIGGER=1;          //Dar inicio al sensor
  __delay_us(10);
  TRIGGER=0;
  etimeout=1;         //Se habilita la condici�n de antibloqueo
  while(ECHO==0  && etimeout==1); //Se espera que el sensor empiece la
                      //medici�n o que pase el antibloqueo (aprox 2s)
  if(etimeout==0){    //Si el sensor no responde se retorna un 0
    return 0;
  }    
  TMR1ON=1;           //Se da inicio al timer1 o medici�n de tiempo
  while(CCP2IF==0 && TMR1IF==0);   //Espera a que la se�al de ultrasonido regrese
  TMR1ON=0;           //Se da parada al timer 1 o medici�n de tiempo
  if(TMR1IF==1)       //Se comprueba que la medici�n del pulso del sensor no
    aux=255;          //exceda el rango del timer1, si es asi se limita a 255
  else{  
    if(CCPR2>=154)  //Si el sensor excede 11cm se limita a este valor
      CCPR2=154;
    aux=CCPR2/(14) ; //Se calcula el valor de distancia a partir del tiempo
  }
  
  return aux;         //Se retorna la medici�n de distancia obtenida
}



void EEPROM_Write(unsigned char address, unsigned char data) {
    EEADR = address;   // Direcci�n de EEPROM
    EEDATA = data;     // Dato a escribir
    EECON1bits.EEPGD = 0;  // Acceso a EEPROM (no a la memoria de programa)
    EECON1bits.WREN = 1;   // Habilitar escritura

    // Secuencia de desbloqueo
    INTCONbits.GIE = 0;    // Deshabilitar interrupciones
    EECON2 = 0x55;         // Secuencia obligatoria
    EECON2 = 0xAA;         // Secuencia obligatoria
    EECON1bits.WR = 1;     // Iniciar escritura
    INTCONbits.GIE = 1;    // Habilitar interrupciones

    while (EECON1bits.WR); // Esperar a que termine la escritura
    EECON1bits.WREN = 0;   // Deshabilitar escritura
}

unsigned char EEPROM_Read(unsigned char address) {
    EEADR = address;   // Direcci�n de EEPROM
    EECON1bits.EEPGD = 0;  // Acceso a EEPROM (no a la memoria de programa)
    EECON1bits.RD = 1;     // Iniciar lectura
    return EEDATA;         // Devolver el dato le�do
}