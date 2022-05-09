/* Bibliotecas */
#include <avr/interrupt.h>
#include <avr/io.h>

#define TOP 124
#define M 124

/*  Escolha do valor de M e dos registradores de comparação:
    Escolhi uma divisão de 1024 da frequência pelo prescaler e um valor de 124 para TOP.
    Desse modo a frequência da onda PWM é de 125Hz, maior do que a resolução temporal do olho humano (aproximadamente 30Hz)
    Escolhi um valor de 124 para M (valor máximo do registrador que controla o duty cycle) pois 
    isso gera um período de fade in de 1s exato. 
    M precisa ser igual a TOP para que vejamos o LED em toda sua intensidade.
*/

// Iniciar no Fade In
volatile char fadeIn = 1;

// configuracao dos registradores
void setup(){
    // desativa interrupções globais para configuração
    cli();
  
    // Timer/Counter 2
    // Output Compare Register A - Define o Período da Onda PMW
    OCR2A = TOP;
    // Output Compare Register B - Define o Duty Cycle da Onda PWM  (inicialmente 0)
    OCR2B = 0;
    // Timer/Counter 2 Interrupt Mask Register 
    // Timer/Counter2 Interrupt Mask Register
    // - bits 7 a 3: reservados, sempre 0 ('00000')
    // - bit 2: desabilitar interrupcao de comparação com Output A ('0')
    // - bit 1: desabilitar interrupcao de comparação com Output B ('0')
    // - bit 0: habilitar interrução de overflow ('1')
    TIMSK2 = 1;
    // Timer/Counter Control Register B
    // - bits 7 e 6: não forçar comparação ('00')
    // - bits 5 e 4: reservados, sempre 0 ('00')
    // - bit 3: modo de operacao Fast PWM com TOP = OCR2A  ('1')
    // - bits 0, 1 e 2: configuração do prescaler: dividir a frequência por 1024  ('111')
    TCCR2B = 0x0F;
    // Timer/Counter Control Register A
    // - bits 7 e 6: desconectar OC0A ('00')
    // - bits 5 e 4: conectar OC0B no modo "non-inverting" ('10') - saida do PWM
    // - bits 3 e 2: reservados, sempre 0 ('00')
    // - bits 0 e 1: modo de operacao Fast PWM com TOP = OCR2A ('11')
    TCCR2A = 0x23;
  
    // PORTB
    // Configurar PB5 (Led embutido) como output: setar o bit 5 como 1
    DDRB |= (0x20);
    // LED embutido inicia ligado (fade in)
    PORTB |= (0x20);
    
    // PORTD
    // Configurar PD3 como output: setar o bit 3 como 1
    DDRD |= (0x08);

    // ativa interrupções globais
    sei();
    return;
}

// Tratamento da interrupcao de overflow (1 perido do PWM)
ISR(TIMER2_OVF_vect){
    if (fadeIn && OCR2B < M)
        OCR2B++;
    else if (!fadeIn && OCR2B > 0)
        OCR2B--;
    else {
        if (fadeIn)
          fadeIn = 0;
        else
          fadeIn = 1;
        PORTB ^= (0x20);
    }
}

int main(){
    setup(); // configura os registradores
    while(1){
      _delay_ms(1); // Delay para melhorar o comportamento da simulacao do Tinkercad
    }
    return 0;
}
