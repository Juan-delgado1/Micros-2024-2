include P18F4550.inc
CONFIG FOSC=HS   ;fosc=4MHz   Fbus=1MHz
CONFIG WDT=OFF      ;tosc=0.25us    tbus=1us
CONFIG PBADEN=OFF
CONFIG LVP=OFF

org 0x0
    goto Inicio

aux1 equ 1h
aux2 equ 3h
aux3 equ 2h

Inicio

; salidas del contador
    bcf TRISD, 0
    bcf TRISD, 1
    bcf TRISD, 2
    bcf TRISD, 3

; salidas del rgb
    bcf TRISA, 0
    bcf TRISA, 1
    bcf TRISA, 2

; entradas de las excepciones
    bsf TRISB, 1
    bsf TRISB, 2

    clrf LATA
    call Retardo_2s

Numero_1
    movlw b'0001' ; 1
    movwf LATD
    movlw b'101'
    movwf LATA
    call Retardo_200ms
    btfss PORTB, 1
    call Pausa
    btfsc PORTB, 2
    goto Numero_6
    goto Numero_2

Numero_2
    movlw b'0010' ; 2
    movwf LATD
    movlw b'001'
    movwf LATA
    call Retardo_200ms
    btfss PORTB, 1
    call Pausa
    btfsc PORTB, 2
    goto Numero_1
    goto Numero_3

Numero_3
    movlw b'0011' ; 3
    movwf LATD
    movlw b'011'
    movwf LATA
    call Retardo_200ms
    btfss PORTB, 1
    call Pausa
    btfsc PORTB, 2
    goto Numero_2
    goto Numero_4

Numero_4
    movlw b'0100' ; 4
    movwf LATD
    movlw b'010'
    movwf LATA
    call Retardo_200ms
    btfss PORTB, 1
    call Pausa
    btfsc PORTB, 2
    goto Numero_3
    goto Numero_5

Numero_5
    movlw b'0101' ; 5
    movwf LATD
    movlw b'110'
    movwf LATA
    call Retardo_200ms
    btfss PORTB, 1
    call Pausa
    btfsc PORTB, 2
    goto Numero_4
    goto Numero_6

Numero_6
    movlw b'0110' ; 6
    movwf LATD
    movlw b'100'
    movwf LATA
    call Retardo_200ms
    btfss PORTB, 1
    call Pausa
    btfsc PORTB, 2
    goto Numero_5
    goto Numero_1

;-------------------------------------------

Pausa
    movlw b'111'
    movwf LATA
    call Retardo_5s
    return
;----------------------------------------------

Retardo_200ms
	movlw .64
	movwf aux1  ;10+((aux1*3+5)*aux2+5)*aux3
	movlw .145
	movwf aux2
	movlw .7
	movwf aux3

AuxRetardo_200ms
	decfsz aux1,f
	goto AuxRetardo_200ms
	movlw .64
	movwf aux1
	decfsz aux2,f
	goto AuxRetardo_200ms
	movlw .145
	movwf aux2
	decfsz aux3,f
	goto AuxRetardo_200ms
	return

;-------------------------------------
Retardo_5s
	movlw .230
	movwf aux1
	movlw .218
	movwf aux2
	movlw .33
	movwf aux3

AuxRetardo_5s
	decfsz aux1,f
	goto AuxRetardo_5s
	movlw .230
	movwf aux1
	decfsz aux2,f
	goto AuxRetardo_5s
	movlw .218
	movwf aux2
	decfsz aux3,f
	goto AuxRetardo_5s
	return

;-----------------------------------------
Retardo_2s
	movlw .192
	movwf aux2
	movlw .24
	movwf aux3
    movlw .143
	movwf aux1
	nop
AuxRetardo_2s
	decfsz aux1,f
	goto AuxRetardo_2s
	movlw .143
	movwf aux1
	decfsz aux2,f
	goto AuxRetardo_2s
	movlw .192
	movwf aux2
	decfsz aux3,f
	goto AuxRetardo_2s
	return

end
    


