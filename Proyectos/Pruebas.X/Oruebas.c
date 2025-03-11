#include <xc.h>

#define _XTAL_FREQ 1000000

// Configuración de bits (ajustar según tu proyecto)
#pragma config FOSC = INTOSC_EC // Oscilador externo de alta frecuencia con PLL habilitado
#pragma config WDT = OFF      // Watchdog Timer desactivado
#pragma config LVP = OFF      // Programación en bajo voltaje desactivada


unsigned char contadas;
void EEPROM_Write(unsigned char address, unsigned char data);
unsigned char EEPROM_Read(unsigned char address);
void MostrarNumeros(void);

void main(void) {
    TRISD = 0;  // Configurar RB0 como salida
    TRISE = 0;
        LATD = 1;   // Encender LED
        LATD=1;
        contadas = 0;
    unsigned char data_written = 19; // Dato a escribir en EEPROM
    unsigned char data_read;
    unsigned char address = 0x10;      // Dirección en EEPROM (0x00 - 0xFF)
    unsigned char apagar;

    
    if(POR == 0){
        data_read = EEPROM_Read(address);
        POR = 1;
    }  else {
        data_read = EEPROM_Read(1);
    }
    // Escribir el dato en la EEPROM  
    

    // Leer el dato de la EEPROM
    
    
    

    while(1){
        
        contadas = data_read;
        MostrarNumeros();
        EEPROM_Write(address, 4);
        
    }
    

    
}

void EEPROM_Write(unsigned char address, unsigned char data) {
    EEADR = address;   // Dirección de EEPROM
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
    EEADR = address;   // Dirección de EEPROM
    EECON1bits.EEPGD = 0;  // Acceso a EEPROM (no a la memoria de programa)
    EECON1bits.RD = 1;     // Iniciar lectura
    return EEDATA;         // Devolver el dato leído
}

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

}

