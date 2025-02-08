/*      Estructura código en C        */

/*      1. Librerias      */

#include <xc.h>     // Libreria de todos los pics
#include <stdio.h>  // Manejo de memoria, conversion tipos de datos, entre otros
#include <conio.h>  // Manejo de apuntadores
#include <stdlib.h> // Manejo cadenas de texto (concatenaciones)
#include <math.h>   // Operaciones y funciones matemáticas

#define _XTAL_FREQ 48000000 // Frecuencia del reloj en Hz

// librerias iostream No se encuentran en microcontroladores, no tiene pantallas o concola para mostrar
// Es mejor no llamar librerias que no se usen


/*      2. Configuraciones generales       */
// Se utilizan las mismas palabras que en ensamblador

#pragma config FOSC = INTOSC_EC
#pragma config WDT = OFF


/*      3. Creación de variables       */
// Globales

unsigned char b = 0;     // 8 bits <0-127> , no necesariamente para caracteres
int a;          // 16 bits <0-65535>
long c;         // 32 bits
float d;        // 24 bits nomenclatura IEEE
double e;       // 24 bits igual a float. SOLO ELEGIR UNO EN CADA CODIGO   
bit f;          // toma valores 0 y 1 pero ocupa 8 bits, tiene ciertas condiciones
// No existen las cadenas de texto como tipo básico de dato
// Se pueden inicializar cuando se crean
// Es importante optimizar la memoria y elegir una variable que sea suficiente y no utilice tantos bits
// Sensible a las mayúsculas
char A[3];      // Arreglos
int  B[2][3];   // Matrices, no hay matrices 3D, nxm posiciones de 16 bits
char *g;        // Apuntadores, apunta a una variable tipo char, no se puede decir que tamaño tiene, 16 bits
int *h;         // Sirven para estructuras, pilas, colas, árboles, vectores


/*      4. Instrucciones - Código        */
void main(void){ //Funcion principal: no recibe nada, no entrega nada
    
    NOP();
    
    char a1=6, b1=3, c1;
    c1=a1/b1;
    c1 = a1;
    TRISD0 = 0; // bcf TRISD,0
    while(1){   // Similar a goto Etiqueta. Siempre e repite
        LATD0 = 1;   // bsf LATD,0
        __delay_ms(1000); // Retardo en ms
        LATD0 = 0;   // bsf LATD,0
        __delay_ms(1000); 
    }
}
