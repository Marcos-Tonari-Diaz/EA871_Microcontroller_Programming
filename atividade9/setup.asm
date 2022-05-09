.org 0x0000 ; o programa comeca no endereco 0x0000 (de palavra) da memoria de programa, que corresponde a
jmp init ; salto para evitar a regiao da memoria onde ficam armazenados os vetores de interrupcao
.org 0x0004
jmp rsi
.org 0x0034 ; primeiro endereco (de palavra) livre fora da faixa reservada para os vetores de interrupcao

init:
    ldi r16, 4
    sts counter, r16
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
    
    ; configuracao da interrupcao INT1 do pino 3
    
    ; congigurar registrador EICRA: bit 3 = 1 e bit 2 = 0 (interrupcao na borda de descida)
    ; OBS: como os bits nao reservados 0 e 1 serao ignorados pela mascara, podemos seta-los com 0
    li r16, 0x08 
    sts EICRA, r16
    
    ;congigurar registrador EIMSK: ativar INT1 e desativar INT0 (bit 1=1, bit 0=0)
    sbi EIMSK, 1
    cbi EIMSK, 0
    
    sei ; habilitar interrupcoes globais

    ; contador
    lds r16, counter
    subi r16,1
    sts counter, r16
    cpi r16, 0    
    breq infinite_loop
    call loop
    jmp main
    ;

    ; periodo normal do led (0,5Hz)
    ; acionar o led
    ; contar 
    
    
tempo:
    sbi 0x07,0 ; configura o pino A0 como saida
    cbi 0x0A,7 ; configura o pino 7 como entrada
    ret

infinite_loop:
    jmp infinite_loop
    
; rotina de servico de interrupcao associada a INT1
rsi:
    ; salvar o contexto
    push r16 ; salvar o reg 16 na pilha
    in r16, SREG ; salvar o SREG na pilha
    push r16
    
    
.dseg
counter: .BYTE 1

