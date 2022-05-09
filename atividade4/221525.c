#include <avr/interrupt.h>
#include <util/delay.h>

// intervalo de amostragem do encoder
#define DELTA_T_MS 100
// pulsos por volta do motor
#define PULSES_PER_ROTATION 48
// frequencia em RPM
#define F_RPM(NUM_PULSES) (60000*NUM_PULSES)/(PULSES_PER_ROTATION*DELTA_T_MS)
// retorna a centena da rotação em RPM, a partir de uma rotação em RPS
#define RPM_CENTENA(rpm) rpm/100

// tabela: numeros => representacao em 7 segmentos
unsigned char seg_table[] = {
	//gfedcbap
	0b01111110,	 // 0
	0b00001100,	 // 1
	0b10110110,	 // 2
	0b10011110,	 // 3
	0b11001100,	 // 4
	0b11011010,	 // 5
	0b11111010,	 // 6
	0b00001110,	 // 7
	0b11111110,	 // 8
	0b11011110   // 9
};

// Ponteiros globais para os registradores das portas

//PORTD
unsigned char *p_portd;
unsigned char *p_ddrd;

//PORTC
unsigned char *p_portc;
unsigned char *p_ddrc;
unsigned char *p_pinc;

//Registradores de interrupcao do pino
unsigned char *p_pcicr;
unsigned char *p_pmsk1;

// contador de pulsos
uint16_t pulses = 0;

// realiza o setup de pinos utilizando 
// os registradores das portas
void setup(void){
  
  	// desativa interrupções globais para configuração
  	cli();
  
  	// cria referencias para os registradores
  	// PORTD
  	p_portd = (unsigned char*) 0x2B;
  	p_ddrd = (unsigned char*) 0x2A;
  
  	// PORTC
  	p_portc = (unsigned char*) 0x28;
  	p_ddrc = (unsigned char*) 0x27;
    p_pinc = (unsigned char*) 0x26;
  
  	// Registrador de controle das interrupcoes dos pinos
  	p_pcicr = (unsigned char*) 0x68;
  	// Registrador de mascara para o sinal PCINT1
  	p_pmsk1 = (unsigned char*) 0x6C;
  
  	//Configuração dos registradores:
  
  	// PC0: configurar como entrada
  	// para que o software nao dispare uma interrupcao inadivertida
  	// input: setar o bit 0 do DDR como 0 (e nao mexer nos outros)
  	*p_ddrc &= ~(0x01);
  
  	// PDx: sete segmentos + ponto decimal
  	// output: setar os bits do DDR como 1, menos DDRD0 (ponto decimal)
  	*p_ddrd |= ~(0x01);
  	// inicializar os segmentos desligados: setar os bits do Data Register como 0, menos PORTD0 (ponto decimal)
  	*p_portd &= 0x01;
  
  	// Habilita a interrupção Pin Change PCINT1 no PCICR
 	*p_pcicr |= 0x2;
  	// Habilita o disparo da interrupção PCINT1 somente pelo pino PCINT8 (pino 23)
  	*p_pmsk1 |= 0x1;
  	
    // ativa interrupções globais
  	sei();
  	return;
}

// Rotina de Tratamento da Interrupcao (com o argumento do sinal PCINT1)
ISR(PCINT1_vect){
	// captura a borda de subida: verifico se o bit0 é alto
  	if (*p_pinc % 2){
      	// incremento o número de pulsos
    	pulses++;
  	}
}

int main(){
  // variável que guarda a frequência em RPM
  uint16_t f_rpm;
  setup();
  while(1){
    // espero um intervalo de amostragem dos pulsos
    // a contagem ocorre na rotina de serviço de interrupção (ISR) somente durante esse intervalo
  	_delay_ms(DELTA_T_MS);
    // desabilito as interrupções antes de continuar,
    // pausando a contagem de pulsos
    cli();
    // atualiza a frequência
    f_rpm = F_RPM(pulses);
    // atualiza o display com a centena da velocidade em rpm
    *p_portd = seg_table[RPM_CENTENA(f_rpm)];
    // habilita as interrupções globais, iniciando uma nova contagem
    sei();
    pulses=0;
  }
  return 0;
}