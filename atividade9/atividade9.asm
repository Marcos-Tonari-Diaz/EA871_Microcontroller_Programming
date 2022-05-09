.org 0x0000 ; o programa comeca no endereco 0x0000 (de palavra) da memoria de programa, que corresponde a
jmp main ; salto para evitar a regiao da memoria onde ficam armazenados os vetores de interrupcao
.org 0x0004 ; endereco para onde o PC vai quanto ocorre a interrupcao INT1
jmp rsi
.org 0x0034 ; primeiro endereco (de palavra) livre fora da faixa reservada para os vetores de interrupcao

main:
    cli ; desabilitar interrupcoes globais
    
    ; iniciando o stack pointer em 0x08FF (ultimo endereco da memoria)
    ldi r16, 0xFF ; carregar byte inferior
    out SPL, r16
    ldi r16, 0x08 ; carregar byte superior
    out SPH, r16
    
    ; configuracoes das portas
    sbi DDRB, 5 ; configurar o led embutido como saida
    cbi DDRD, 3 ; configurar o pino 3 (PD3) como entrada (botao)
    cbi PORTB, 5 ; zerar o led embutido no inicio
    ldi r16, 0 ; salvar estado incial do led na variavel
    sts led_on, r16

    
    ; configuracao da interrupcao INT1 do pino 3
    
    ; congigurar registrador EICRA: bit 3 = 1 e bit 2 = 0 (interrupcao na borda de descida)
    ; OBS: como os bits nao reservados 0 e 1 serao ignorados pela mascara, podemos seta-los com 0
    ldi r16, 0x08 
    sts EICRA, r16
    
    ; congigurar registrador EIMSK: ativar INT1 e desativar INT0 (bit 1=1, bit 0=0)
    sbi EIMSK, 1
    cbi EIMSK, 0
    
    sei ; habilitar interrupcoes globais

    ; loop para piscar o led com frequencia de 0,5Hz
    led_loop:
	; chama funcao de atraso
	ldi r22, 100 ; "argumento" da funcao (numero de intervalos de 10ms)
	call timer_loop
	; verifica estado do LED (variavel de 1 byte led_on)
	lds r16, led_on
	ldi r17, 1
	cp r16, r17
	breq desliga_led  ; se o led_esta ligado, desligue-o. Caso contrario, ligue-o
	liga_led:
	    sbi PORTB, 5 ; seta o led embutido (pino PB5)
	    ldi r16, 1 ; grava novo estado na variavel
	    sts led_on, r16 
	    jmp fim_comparacao
	desliga_led:
	    cbi PORTB, 5 ; apaga o led embutido (pino PB5)marcos.dure.diaz@gmail.com
	    ldi r16, 0 ; grava novo estado na variavel
	    sts led_on, r16
	fim_comparacao:
	    jmp led_loop
    
; rotina de servico de interrupcao associada a INT1
rsi:
    ; salvar o contexto
    push r16 ; salvar o reg 16 na pilha
    push r17 ; salvar o reg 16 na pilha
    push r18 ; salvar o reg 16 na pilha
    push r19 ; salvar o reg 16 na pilha
    push r20 ; salvar o reg 16 na pilha
    push r21 ; salvar o reg 16 na pilha
    push r22 ; salvar o reg 16 na pilha
    in r16, SREG ; salvar o SREG na pilha
    push r16
    
    ; sequencia do botao
    ; delay de 100ms
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    cbi PORTB, 5 ; apaga o led embutido (pino PB5)
    
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    sbi PORTB, 5 ; apaga o led embutido (pino PB5)
    
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    cbi PORTB, 5 ; apaga o led embutido (pino PB5)
    
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    sbi PORTB, 5 ; apaga o led embutido (pino PB5)
    
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    cbi PORTB, 5 ; apaga o led embutido (pino PB5)
    
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    sbi PORTB, 5 ; apaga o led embutido (pino PB5)
    
    ldi r22, 10 ; "argumento" da funcao (numero de intervalos de 10ms)
    call timer_loop
    cbi PORTB, 5 ; apaga o led embutido (pino PB5)

    ; restaura o estado do LED
    ; verifica estado do LED (variavel de 1 byte led_on)
    lds r16, led_on
    ldi r17, 1
    cp r16, r17
    breq r_liga_led  ; verifica se deve ligar ou desligar
    r_desliga_led:
        cbi PORTB, 5 ; apaga o led embutido (pino PB5)marcos.dure.diaz@gmail.com
        ldi r16, 0 ; grava novo estado na variavel
        sts led_on, r16
	jmp fim_rsi
    r_liga_led:
        sbi PORTB, 5 ; seta o led embutido (pino PB5)
        ldi r16, 1 ; grava novo estado na variavel
        sts led_on, r16 


fim_rsi:
    ; recupera o contexto
    pop r16
    out SREG, r16
    pop r22
    pop r21
    pop r20
    pop r19
    pop r18
    pop r17
    pop r16
    
    sbi EIFR, 1 ; limpa flag INT1 (evitar interrupcao espuria)

    reti
    
; rotina de delay:
; timer_loop foi projetado para que cada ciclo
; do contador a tenha 32 ciclos de relogio e 100us
; e um ciclo do contador b tenha 10ms
; o numero de ciclos do contador c define o tempo total em multiplos de 10ms
; (contador b conta contador a, contador c conta contador b)
timer_loop:
    ; OBS: optei por nao salvar os registradores do programa principal, pois nao serao mais usados
    ; registradores utilizados r16-r21
    ldi r16, 0 ; a conta 100 us (32 ciclos, 50 vezes)
    ldi r17, 0 ; b conta 100*100us
    ldi r18, 0 ; c conta 100*100*100us = 1s
    ldi r19, 1 ; constante
    ldi r20, 50 ; constante 
    ldi r21, 100 ; constante 
    
    mid_loop: ;loop para gastar tempo e incrementar os contadores
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
	cp r18, r22 ; compara c com argumento
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
	ret

.DSEG
led_on: .BYTE 1 ;