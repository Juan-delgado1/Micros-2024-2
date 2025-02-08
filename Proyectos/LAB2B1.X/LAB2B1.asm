include P18F4550.inc

CONFIG FOSC = EC_EC ; Reloj Externo
		    ; _EC => Oscilador del USB que se configura así no se use
CONFIG WDT = OFF    ; Perro Guardian
CONFIG LVP = OFF    ; Funcionamiento en Bajo Voltaje

Inicio
   clrf TRISD	; [1] Posterior a esto se ponen todos los pines del puerto D como salidas

Menu   
   ; Inicia Negro
   ;0 verde, 1 rojo, 2 azul
   clrf LATD	; [1] Se pone como valor 0 a todos los pines del puerto D para evitar afectar la secuencias
   bsf LATD, 2	; [1] Azul
   bsf LATD, 0  ; [1] Cyan
   bcf LATD, 2	; [1] Verde
   bsf LATD, 1	; [1] Amarillo
   bsf LATD, 2	; [1] Blanco
   bcf LATD, 0	; [1] Magenta
   bcf LATD, 2	; [1] Rojo
   bra Menu	; [2] Vuelve a la etiqueta Inicio
end