#include <util/delay.h>

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
	0b11011110,	 // 9
    0b11101110,	 // a
    0b11111000,	 // b
    0b01110010,	 // c
    0b10111100,	 // d
    0b11110010,	 // e
    0b11100010	 // f
};

// Ponteiros globais para os registradores das portas

//PORTC
unsigned char *p_portc;
unsigned char *p_ddrc;
unsigned char *p_pinc;

//PORTD
unsigned char *p_portd;
unsigned char *p_ddrd;

// realiza o setup de pinos utilizando 
// os registradores das portas
void setup(void){
  	// cria referencias para os registradores
	// PORTC
  	p_portc = (unsigned char*) 0x28;
  	p_ddrc = (unsigned char*) 0x27;
  	p_pinc = (unsigned char*) 0x26;
  
  	// PORTD
  	p_portd = (unsigned char*) 0x2B;
  	p_ddrd = (unsigned char*) 0x2A;
  	
    // PC0: push-button
  	// input: setar o bit 0 do DDR como 0 (e nao mexer nos outros)
  	*p_ddrc &= ~(0x01); 
  	// ativar pull-up resistor: setar o bit0 do Data Register com 1 (e nao mexer nos outros)
    *p_portc |= 0x01;
  	
  	// PDx: sete segmentos + ponto decimal
  	// output: setar os bits do DDR como 1
  	*p_ddrd |= ~(0x00);
  	// inicializar os segmentos desligados: setar os bits do Data Register como 0
  	*p_portd &= 0x00;
  
  	return;
}

int main(){
  setup();
  // contador do push-button
  unsigned char counter = 0;
  // inicializa o display com o 0
  *p_portd |= seg_table[counter];

  // amostras do estado do push-button (inicialmente aberto)
  unsigned char first_sample = 0x01;
  unsigned char second_sample = 0x01;
  
  //variavel de estado do botao: 0 esta pressionado, 1 aberto
  unsigned char state = 1;
  
  // a cada ciclo, leio o push-button e atualizo o display
  while(1){
    //debouncing
    // primeira leitura do push-button: a operacao & 0x01 descarta todos os bits menos o bit0
  	first_sample = *p_pinc & 0x01;
    // esperar o transitorio do push_button
    _delay_ms(10);
    // segunda leitura
  	second_sample = *p_pinc & 0x01;
    // se houve mudança proposital no estado do botão 
    // (caso contrario, mantemos o estado e pulamos para a proxima iteracao)
    if(first_sample==second_sample){
      // atualizamos o estado do botao caso ele tenha mudado
      if (state != first_sample){
        state = first_sample;
      	// se foi pressionado, atualizamos o contador e ligamos o ponto decimal
      	if (state == 0){
        	// liga o ponto decimal
        	*p_portd |= 0x01;
        	// incrementamos o contador
      		if (counter < 15) counter++;
      		// resetamos o contador se ele passar de 15
      		else counter=0;
      	}
      	// caso contrario, desligamos o ponto decimal
      	else *p_portd &= ~(0x01);
      	// atualiza o display: encontramos a sequencia de segmentos 
        // correspondentes ao numero counter,
        // preservando o estado do ponto decimal
        // (bit0 do registrador)
      	*p_portd = seg_table[counter] | ((*p_portd) % 2);
      }
    }
  }
  return 0;
}