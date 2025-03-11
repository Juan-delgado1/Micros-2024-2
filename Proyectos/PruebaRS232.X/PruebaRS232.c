//Fosc=1MHz
//BaudRate 9600 bps
#include<xc.h>
#include <stdio.h> 
#include <stdlib.h>
#define _XTAL_FREQ 1000000

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

void Transmitir(unsigned char);
unsigned char Recibir(void);
void putch(char data);

void main(void){
    TRISD = 0;  // Puerto D como salida
    LATD = 0;
    unsigned char contador = 0;

    // Configurar UART
    BAUDCON = 0b00001000;  // BRGH16 = 1
    SPBRG = 25;            // 9600 bps con Fosc = 1MHz
    TXSTA = 0b00100100;    // TXEN = 1, BRGH = 1, SYNC = 0
    RCSTA = 0b10010000;    // SPEN = 1, CREN = 1 (Habilita recepción continua)
    unsigned char a = 0;
    while(1){
        //Recibe();
        
        
        //__delay_ms(250);

        //a = Recibir();  // Convertir ASCII a número
        //LATD = a;
        //Transmitir(1);

        //Transmitir(1);
        unsigned char pwm = 2;
        //printf("Valor de PWM: %.2f %%\r",pwm);
    }
}

unsigned char Recibir(void){
    while(RCIF == 0);  // Esperar a recibir un dato
    return RCREG;
}

void Transmitir(unsigned char BufferT){
    while(TRMT == 0);  // Esperar a que se vacíe el buffer de transmisión
    TXREG = BufferT + 48;        // Transmitir en ASCII
}

void putch(char data) {//Necesaria cuando se usa "printf()"
    while(TRMT == 0);  //Espera hasta que el registro de transmisión esté vacío
    TXREG = data;      //Carga el carácter a transmitir en el registro de transmisión
}
