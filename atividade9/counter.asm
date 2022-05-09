.org 0x0000 ; o programa comeca no endereco 0x0000 (de palavra) da memoria de programa, que corresponde ao
jmp init ; salto para evitar a regiao da memoria onde ficam armazenados os vetores de interrupcao
.org 0x0034 ; primeiro endereco (de palavra) livre fora da faixa reservada para os vetores de interrupcao

init:
    ldi r16, 4
    sts counter, r16
main:
    lds r16, counter
    subi r16,1
    sts counter, r16
    cpi r16, 0    
    breq infinite_loop
    call loop
    jmp main

loop:
    sbi 0x07,0 ; configura o pino A0 como saida
    cbi 0x0A,7 ; configura o pino 7 como entrada
    ret

infinite_loop:
    jmp infinite_loop
.dseg
counter: .BYTE 1

