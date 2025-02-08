; Primera Seccion - Librerías
include P18f4550.inc

			    
; Segunda Seccion - Configuracion Microcontrolador, aún no se necesita

; Tercera seccion - Crear variables
aux1 equ 0h
aux2 equ 1h    ; siempre debe tener diferente direccion de memoria
aux3 equ 2h
sol equ 3h
 
 ;0x prefijo hexadecimal
 ;b prefijo binaro
 ; . para numeros decimales


; Cuarta seccion - Desarrollo
Inicio
 
    
    ; Primer punto
    movlw .5	; W <- 5
    movwf aux1	; aux1 <- W
    
    ; Segundo punto
    movlw .9
    movwf aux1
    
    ;movlw .256 ; EL número máximo que se puede guardar es 255, con 256 el valor no se guarda
    ;movwf var1
    
    ; Tercer punto
    
    movf aux1,w
    addlw .5
    movwf aux2
    
    
    ; Cuarto Punto
    
    ;op1: Sumar 3 a la variable aux1 con el valor de 7.
    movlw .7
    movwf aux1
    addlw .3
    movwf sol
    
    ;op2: Sumar aux1 = 8 y aux2 = 10
    movlw .8
    movwf aux1
    movlw .10
    movwf aux2
    movf aux1, w
    addwf aux2, w 
    movwf sol
    
    ;op3: Restar a la constante 9 la variable aux1 con el valor de 5
    movlw .5
    movwf aux1 
    sublw .9
    movwf sol	  
    
    
    ;op4: Restar la variable aux1 con valor de 6 a la variable aux2 con valor de 4
    movlw .4
    movwf aux2
    movlw .6
    movwf aux1
    movf aux1,w
    subwf aux2, w
    movwf sol
    
    ;op5: Multiplicar a la constante 4 la variable aux1 cargada previamente con el valor de 5.
    movlw .5
    movwf aux1
    mullw .4
    movf PRODL, w
    movwf sol
    
    ;op6: Multiplicar la variable aux1 a la variable aux2 previamente cargadas con los valores de 12 y 15 respectivamente.
    movlw .12
    movwf aux1
    movlw .15
    movwf aux2
    mulwf aux1
    movf PRODL, w
    movwf sol
    
    ;op7: Realizar el complemento a 1 de la variable aux1 cargada previamente con el valor de 12.
    movlw .12
    movwf aux1
    comf aux1
    
    
    ;op8: Realizar el complemento a 2 de la variable aux1 cargada previamente con el valor de 12.
    movlw .12
    movwf aux1
    negf aux1
    movf aux1, w 
    movwf sol 
    
    ;op9:
    movlw .35    
    movwf aux1    ;00100011 
    iorlw .7      ;00000111
    movwf sol     ;00100111
    
    ;op10: 
    movlw .20
    movwf aux1
    movlw .56
    movwf aux2       ;00111000
    iorwf aux1, w    ;00010100
    movwf sol        ;00111100
    
    ;op11: 
    movlw .62
    movwf aux1 ;00111110
    andlw .15  ;00001111
    movwf sol  ;00001110
    
    
    ;op12: 
    movlw .100
    movwf aux1
    movlw .45
    movwf aux2    ;00101101
    andwf aux1, w ;01100100
    movwf sol	  ;00100100
    
    ;op13: 
    movlw .120;01111000
    movwf aux1
    xorlw .1 ;00000001
    movwf sol;01111001
    
    
    ;op14: 
    movlw .17
    movwf aux1;00010001
    movlw .90
    movwf aux2;01011010
    xorwf aux1, w;01001011
    movwf sol
    
    ;op15: sol <-- (aux1 OR aux2) AND (aux3 XOR 0xD0)
    movlw .25
    movwf aux1
    movlw .40
    movwf aux2
    movlw .103
    movwf aux3    
    ;aux1 or aux2
    movf aux1, w 
    iorwf aux2, w  ; 00011001 or 00101000 = 00111001
    movwf sol    
    ; aux3 xor 0xD0 (208)
    movf aux3, w
    xorlw 0xD0 ; 01100111 xor 11010000 = 10110111
    ; w and sol
    andwf sol, w
    movwf sol ; 00111001 and 10110111 = 00110001
    
    
    
    ;op16: aux4 <-- (aux1 + aux2) - 3 * (aux3 - 0b11010)
    movlw .18
    movwf aux1
    movlw .60
    movwf aux2
    movlw .16
    movwf aux3
    
    ;aux1+aux2
    movf aux1, w
    addwf aux2, w
    movwf sol    ;78
    
    ;-3*(aux3-0b11010)--(26)
    movlw b'11010'
    subwf aux3, w
    movwf aux3 ;"246"
    negf aux3 ; "10"
    movf aux3, w
    mullw .3 
    movf PRODL, w ;"30"
    
    ; sol + w
    addwf sol, w
    movwf sol; 108
    
    
    ;op16: aux4 <-- (aux1 + aux2) - 3 * (aux3 - 0b11010)
    movlw .18
    movwf aux1
    movlw .60
    movwf aux2
    movlw .16
    movwf aux3    
    ;aux1+aux2
    movf aux1, w
    addwf aux2, w
    movwf sol    ;78    
    ;-3*(aux3-0b11010)--(26)
    movlw b'11010'
    subwf aux3, w ;"246"
    mullw .3      ;"PRODH = 2" "PRODL = 226"
    negf PRODL    ;"30"
    movf PRODL, w
    ; sol + w
    addwf sol, w
    movwf sol; 108  
    ;GOTO
    goto Inicio
    
end
    
    
    