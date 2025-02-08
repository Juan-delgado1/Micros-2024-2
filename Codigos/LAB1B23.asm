include P18F4550.inc

CONFIG FOSC = EC_EC	; Reloj Externo
CONFIG WDT = OFF	; Perro Guardian
CONFIG LVP = OFF	; Funcionamiento en Bajo Voltaje
CONFIG PBADEN = OFF	 ;Desactivar funciones analógicas
  
aux1 equ 0h
aux2 equ 1h
 
var1 equ 2h
    
; Vectores
ORG 0h   ; que hace si se reinicia el microcontrolador, no es necesario solo si se usan interrupciones
 goto Inicio
 
ORG 8h   ; que hacer si hay una interrupción
 goto ISR

Inicio
  clrf LATD	; [1] Se pone como valor 0 a todos los pines del puerto D para evitar afectar la secuencias
  clrf TRISD	; [1] Posterior a esto se ponen todos los pines del puerto D como salidas
  bsf TRISB,0   ; [1] Modifica como entrada el pin RB0
  
  bsf INTCON2, INTEDG0 ; Definir flanco subida
  bcf INTCON,1  ; Borrar bandera => borro en el registro intcon el pin 1 (INT0IF)   ===  bcf INTCON, INT01F
  bsf INTCON, INT0IE  ; Habilitar la interrupcion local  bit4
  bsf INTCON, GIE     ; Habilitación global interrupciones bit 7
  
Menu   
   ;0 verde, 1 rojo, 2 azul
   clrf LATD	; [1] Negro
   movlw .248
   movwf var1
   nop
   call Retardo   
   
   
   bsf LATD, 2	; [1] Azul
   movlw .81
   movwf var1
   call Retardo
   
   bsf LATD, 0  ; [1] Cyan
   movlw .165
   movwf var1
   call Retardo
   
   bcf LATD, 2	; [1] Verde
   movlw .81
   movwf var1
   call Retardo
   
   bsf LATD, 1	; [1] Amarillo
   movlw .165
   movwf var1
   call Retardo
   
   bsf LATD, 2	; [1] Blanco
   movlw .248
   movwf var1
   call Retardo
   
   bcf LATD, 0	; [1] Magenta
   movlw .165
   movwf var1
   call Retardo
   
   bcf LATD, 2	; [1] Rojo
   movlw .81
   movwf var1
   call Retardo
  
   bra Menu	; [2] Vuelve a la etiqueta Menu 
   
ISR
  bcf INTCON, INT0IF	; [1]    Desactivar Bandera
  btfsc PORTB,0		; [1] o [3]  Si es 0 salta la siguiente instrucción
  goto ISR		; [2]
  retfie		; [2]    Salir de la interrupcion
  ; Al desactivar el interruptor, para volver a su ciclo principal, demorara entre 6 y 10 ciclos de bus
  
Retardo
    movf var1, w
    movwf aux1
AuxRetardo
    decfsz aux1,f
    goto AuxRetardo
    return

  
end


