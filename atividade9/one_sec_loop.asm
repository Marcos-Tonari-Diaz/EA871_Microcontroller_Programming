.org 0x0000 ; o programa comeca no endereco 0x0000 (de palavra) da memoria de programa, que corresponde ao
jmp one_sec_loop ; salto para evitar a regiao da memoria onde ficam armazenados os vetores de interrupcao
.org 0x0034 ; primeiro endereco (de palavra) livre fora da faixa reservada para os vetores de interrupcao

    
; one_sec_loop foi projetado para que cada ciclo
; do contador a tenha 32 ciclos de relogio e 100us
; e um ciclo do contador c tenha 1 segundo
; (contador b conta contador a, contador c conta contador b)
one_sec_loop:
    ; registradores utilizados r16-r21
    ldi r16, 0 ; a conta 100 us (32 ciclos, 50 vezes)
    ldi r17, 0 ; b conta 100*100us
    ldi r18, 0 ; c conta 100*100*100us = 1s
    ldi r19, 1 ; cte 
    ldi r20, 50 ; cte 
    ldi r21, 100 ; cte 
    
    mid_loop: ;loop init dura 2us
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo    
	add r30, r30 ; lixo  
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	
	add r16, r19 ; a++ (incrementa a)
	cp r16, r20 ; compara a com 50
	breq top_a
	cp r17, r21 ; compara b com 100
	breq top_b
	cp r18, r21 ; compara c com 100
	breq top_c
	
	jmp mid_loop
	
    top_a:
	; padding de tempo
	add r30, r30 ; lixo
	add r30, r30 ; lixo
	; fecha padding de tempo
	
	ldi r16, 0 ; a=0 (reset a)
	add r17, r19 ; b++ (incrementa b)
	jmp mid_loop
    top_b:
	ldi r17, 0 ; b=0 (reset b)
	add r18, r19 ; c++ (incrementa c)
	jmp mid_loop
    top_c:
	jmp infinite_loop
    
infinite_loop:
    jmp infinite_loop

