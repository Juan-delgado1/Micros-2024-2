#ifndef LIBLCDXC8_H
#define	LIBLCDXC8_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif
#include<xc.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif
#ifndef Datos
#define Datos LATD	//El puerto de conexión de los datos el cual se puede cambiar
#endif
#ifndef RS
#define RS LATA4	//Los pines de control al LCD los cuales se
#endif
#ifndef E
#define E LATA5	//pueden cambiar
#endif

unsigned char interfaz=8;

void ConfiguraLCD(unsigned char);
void RetardoLCD(unsigned char);
void EnviaDato(unsigned char);
void InicializaLCD(void);
void HabilitaLCD(void);
void BorraLCD(void);
void CursorAInicio(void);
void ComandoLCD(unsigned char);
void EscribeLCD_c(unsigned char);
void EscribeLCD_n8(unsigned char, unsigned char);
void EscribeLCD_n16(unsigned int, unsigned char);
void EscribeLCD_d(unsigned char);
void MensajeLCD_Var(char *);
void DireccionaLCD(unsigned char);
void FijaCursorLCD(unsigned char,unsigned char);
void DesplazaPantallaD(unsigned char);
void DesplazaPantallaI(unsigned char);
void CaracterSp1(void);
void CaracterSp2(void);

void ConfiguraLCD(unsigned char a){
	if(a==4 | a ==8)
		interfaz=a;	
}
void EnviaDato(unsigned char a){
	if(interfaz==4){
		Datos=(Datos & 0b00001111) | (a & 0b11110000);
		HabilitaLCD();
		RetardoLCD(1);
		Datos=(Datos & 0b00001111) | (a<<4);
		//HabilitaLCD();
		//RetardoLCD(1);
	}else if(interfaz==8){
		Datos=a;
	}	
}
void InicializaLCD(void){
//Función que inicializa el LCD caracteres
	RS=0;
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(1);
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(3);
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(4);
	if(interfaz==4){
		Datos=(Datos & 0b00001111) | 0x20;
		HabilitaLCD();
		RetardoLCD(4);
		EnviaDato(0x2F);
		HabilitaLCD();
		RetardoLCD(4);
	}
	BorraLCD();
	EnviaDato(0xF);
	HabilitaLCD();
	RetardoLCD(4);	
}
void HabilitaLCD(void){
//Función que genera los pulsos de habilitación al LCD 	
	E=1;
	__delay_us(40);
    //Delay1TCY();
	E=0;
}
void BorraLCD(void){
//Función que borra toda la pantalla	
	RS=0;
	EnviaDato(0x01);
	HabilitaLCD();
	RetardoLCD(2);
}
void CursorAInicio(){
//Función que lleva el cursor a la primera posición o la de la
//primera línea mas a la izquierda	
}	
void ComandoLCD(unsigned char a){
//Función que envia cualquier comando al LCD
	RS=0;
	if(a==1)
		BorraLCD();
	else if((a&0b11111110)==2)	
		CursorAInicio();
	else{	
		EnviaDato(a);
		HabilitaLCD();
		RetardoLCD(4);
	}		
}	
void EscribeLCD_c(unsigned char a){
//Función que escribe un caracter en la pantalla
//a es un valor en codigo ascii
//Ejemplo EscribeLCD_c('A');
	RS=1;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}
void EscribeLCD_n8(unsigned char a,unsigned char b){
//Función que escribe un número positivo de 8 bits en la pantalla
//a es el número a escribir, el cual debe estar en el rango de 0 a 255
//b es el número de digitos que se desea mostrar empezando desde las unidades
//Ejemplo EscribeLCD_n8(204,3);	
    unsigned char centena,decena,unidad;
	RS=1;
	switch(b){
		case 1: unidad=a%10;
				EnviaDato(unidad+48);
				HabilitaLCD();
				RetardoLCD(4);
				break;
		case 2:	decena=(a%100)/10;
				unidad=a%10;
				EnviaDato(decena+48);
				HabilitaLCD();
				RetardoLCD(4);
				EnviaDato(unidad+48);
				HabilitaLCD();
				RetardoLCD(4);
				break;
		case 3: centena=a/100;
				decena=(a%100)/10;
				unidad=a%10;
				EnviaDato(centena+48);
				HabilitaLCD();
				RetardoLCD(4);
				EnviaDato(decena+48);
				HabilitaLCD();
				RetardoLCD(4);
				EnviaDato(unidad+48);
				HabilitaLCD();
				RetardoLCD(4);
				break;
		default: break;
	}
}
void EscribeLCD_n16(unsigned int a,unsigned char b){
//Función que escribe un número positivo de 16 bits en la pantalla
//a es el número a escribir, el cual debe estar en el rango de 0 a 65535
//b es el número de digitos que se desea mostrar empezando desde las unidades
//Ejemplo EscribeLCD_n16(12754,5);	
    unsigned char decena,unidad;
	unsigned int centena,millar;
    RS=1;
	switch(b){
		case 1: unidad=a%10;
                EscribeLCD_c(unidad+48);
				break;
		case 2:	decena=(a%100)/10;
				unidad=a%10;
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		case 3: centena=(a%1000)/100;
                decena=(a%100)/10;
				unidad=a%10;
                EscribeLCD_c(centena+48);
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		case 4: millar=(a%10000)/1000;
                centena=(a%1000)/100;
                decena=(a%100)/10;
				unidad=a%10;
                EscribeLCD_c(millar+48);
				EscribeLCD_c(centena+48);
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		case 5: EscribeLCD_c(a/10000 +48);
				millar=(a%10000)/1000;
                centena=(a%1000)/100;
                decena=(a%100)/10;
				unidad=a%10;
                EscribeLCD_c(millar+48);
				EscribeLCD_c(centena+48);
				EscribeLCD_c(decena+48);
                EscribeLCD_c(unidad+48);
				break;
		default: break;
	}	
}
void EscribeLCD_d(unsigned char a){
//Función que escribe un caracter en la pantalla
//a es un valor en codigo ascii
//Ejemplo EscribeLCD_c('A');
	RS=1;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}

void MensajeLCD_Var(char* a){
//Función que escribe una cadena de caracteres variable en la pantalla
//a es una cadena de caracteres guardada en una variable *char
//Ejemplo: char aux[4]="Hola"; MensajeLCD_Var(aux);
    while(*a != '\0'){
        EscribeLCD_c(*a);
        a++;
    }
}
void DireccionaLCD(unsigned char a){
//Función que ubica el cursor en una posición especificada
//a debe ser una dirección de 8 bits valida de la DDRAM o la CGRAM	
	RS=0;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}
void FijaCursorLCD(unsigned char fila,unsigned char columna){
//Función que ubica el cursor en una fila y columna especificadas
//fila es un valor positivo que especifica la posición del cursor de 1 a 4
//columna es un valor positivo que especifica la posición del cursor de 1 a 80
//Si el display es de una fila, Ej: 8x1, tiene 80 columnas
//Si el display es de dos filas, Ej: 20x2, tiene 40 columnas
//Si el display es de cuatro filas, Ej: 20x4, tiene 20 columnas
	
}
void DesplazaPantallaD(unsigned char a){
//Función que desplaza una sola vez la pantalla a la derecha
    while(a!=0){
    RS=0;
    ComandoLCD(0x1C);
    RetardoLCD(4);
    a = a-1;
  }	
}
void DesplazaPantallaI(unsigned char a){
//Función que desplaza una sola vez la pantalla a la izquierda
    while(a!=0){
    RS=0;
    ComandoLCD(0x18);
    RetardoLCD(4);
    a = a-1;
    __delay_ms(1000);
  }	
}
void DesplazaCursorD(void){
//Función que desplaza una sola vez la pantalla a la derecha
	
}
void DesplazaCursorI(void){
//Función que desplaza una sola vez la pantalla a la izquierda
	
}		
void RetardoLCD(unsigned char a){
	switch(a){
		case 1: __delay_ms(15);
                //Delay100TCYx(38); //Retardo de mas de 15 ms
				break;
		case 2: __delay_ms(1);
                __delay_us(640);
                //Delay10TCYx(41); //Retardo de mas de 1.64 ms
				break;
		case 3: __delay_us(100);
                //Delay10TCYx(3);	//Retardo de mas de 100 us
				break;
		case 4: __delay_us(40);
                //Delay10TCYx(1); //Retardo de mas de 40 us
				break;
		default:
				break;
	}
}

void CaracterCara(void) {
    ComandoLCD(0b01001000);  // Dirección CGRAM 1
    RS=1;
    RetardoLCD(4);
    EnviaDato(0b11011);
    HabilitaLCD();
    EnviaDato(0b00000);
    HabilitaLCD();
    EnviaDato(0b11011);
    HabilitaLCD();
    EnviaDato(0b11011);
    HabilitaLCD();
    EnviaDato(0b00000);
    HabilitaLCD();
    EnviaDato(0b10001);
    HabilitaLCD();
    EnviaDato(0b01110);
    HabilitaLCD();
    EnviaDato(0b00000);
    HabilitaLCD();
    RS = 0;
    EnviaDato(0xC0);
    HabilitaLCD();
	RetardoLCD(2); 
}

void CaracterQbert(void) {
    ComandoLCD(0b01010000);  // Dirección CGRAM 2
    RS=1;
    RetardoLCD(4);
    EnviaDato(0b11111);
    HabilitaLCD();
    EnviaDato(0b10101);
    HabilitaLCD();
    EnviaDato(0b11111);
    HabilitaLCD();
    EnviaDato(0b11111);
    HabilitaLCD();
    EnviaDato(0b01110);
    HabilitaLCD();
    EnviaDato(0b01010);
    HabilitaLCD();
    EnviaDato(0b11011);
    HabilitaLCD();
    EnviaDato(0b00000);
    HabilitaLCD();
    RS = 0;
    EnviaDato(0xC1);
    HabilitaLCD();
	RetardoLCD(2);
}


    
#endif	/* LIBLCDXC8_H */
