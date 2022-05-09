/* Bibliotecas */
#include <avr/interrupt.h>

/* Tamanho do Buffer */
#define MAX_BUFFER 5

/* Buffer */
volatile char buffer[MAX_BUFFER];
/* Controle de posicao para adicionar valores no buffer */
volatile char add_buf = 0;
/* Controle de posicao para remover valores do buffer */
volatile char del_buf = 0;
/* Numero de posicoes usadas no buffer */
volatile char ocup_buf = 0;

void adicionar_buffer(char c){
  
    /* Se o buffer nao esta cheio */
    if(ocup_buf < MAX_BUFFER){
        /* Adiciona valor no buffer */
      	buffer[add_buf] = c;
      	/* Incrementa o numero de posicoes utilizadas no buffer */
      	ocup_buf++;
        /* Incrementa condicionalmente o controle de posicao para adicionar.
           Se esta na ultima posicao, retorna pra primeira.
           Caso contrario, vai pra posicao seguinte. */
      	if(add_buf == (MAX_BUFFER-1)) add_buf=0;
      	else                          add_buf++;
    }
}

/* Funcao para remover valores do buffer */
char remover_buffer(){
    
    /* variavel auxiliar para capturar o caractere do buffer */
    char c;
  
    /* Se o buffer nao esta vazio */
    if (ocup_buf > 0){
      
    	/* Pega o caractere do buffer */
        c = buffer[del_buf];
      	/* Decrementa o numero de posicoes utilizadas no buffer */
      	ocup_buf--;        
        /* Incrementa condicionalmente o controle de posicao para remover.
           Se esta na ultima posicao, retorna pra primeira.
           Caso contrario, vai pra posicao seguinte. */
      	if(del_buf == (MAX_BUFFER-1)) del_buf=0;
      	else                    	  del_buf++;
    }
    
    return c;
}

// mensagens
char msg_0[] = "Comando: Apagar todos os LEDs\n";
char msg_1[] = "Comando: Varredura com um LED aceso\n";
char msg_2[] = "Comando: Varredura com um LED apagado\n";
char msg_3[] = "Comando: Acender todos os LEDs\n";
char msg_e[] = "Comando incorreto\n";
char msg_v[] = "Vazio!\n";

// Variaveis globais

// Guarda estado do sistema: cada comando valido recebido define um estado
// (Leds inicialmente desligados)
char estado = '0';
// Guarda o ultimo estado
char ultimo = '0';
// Iterador da string
volatile unsigned char it= 0;
// Aponta para a mensagem que esta sendo transmitida
char* msg_atual;

char LEDatual;
char LEDultimo;
char vazio;

// Ponteiros globais para os registradores das portas e da USART

//PORTC
unsigned char *p_portc;
unsigned char *p_ddrc;
unsigned char *p_pinc;

//USART
unsigned char *ubrr0h;
unsigned char *ubrr0l;
unsigned char *ucsr0a;
unsigned char *ucsr0b;
unsigned char *ucsr0c;
unsigned char *udr0;

// configuracao dos registradores
void setup(void){
  
  	// desativa interrupções globais para configuração
  	cli();
  
  	// PORTC
  	p_portc = (unsigned char*) 0x28;
  	p_ddrc = (unsigned char*) 0x27;
  
  	// USART
    ubrr0h = (unsigned char*) 0xC5;    
    ubrr0l = (unsigned char*) 0xC4;
    ucsr0a = (unsigned char*) 0xC0;
    ucsr0b = (unsigned char*) 0xC1;
    ucsr0c = (unsigned char*) 0xC2;
    udr0 = (unsigned char*) 0xC6;
  
  	//Configuração dos registradores:
  	
  	// PORTC
  	// Configurar PC3, PC4 e PC5 como output: setar os bis 3,4 e 5 como 1
  	*p_ddrc |= (0x38);

  	// USART
  	// Baud Rate Registers para 19.2K bps
    *ubrr0h = 0x00;
    *ubrr0l = 0x33;

    // USART Control Status Registers
    // A- bit 1: modo de velocidade - normal ('0')
    //  - bit 0: desabilita a comunicação multi-processador ('0')
    *ucsr0a &= ~(0x03);
    // B- bit 7: habilitar interrupcao de recepcao completea
  	//	- bit 6: desabilitar interrupcao de transmissao completa (inicialmente)
    //  - bit 5: desabilitar a interrupcao de registrador de dados vazio
    //  - bit 4: habilitar o receptor 
    //  - bit 3: habilitar o transmissor 
    //  - bit 2: codigo do numero de bits por frame-usamos 8-'011' ('0')
    //  - bits 1, 0: usado para frames com 9bits (ignorar)
  	//setar os 1s
    *ucsr0b |= 0x98;
    //setar os 0s
    *ucsr0b &= ~(0x64);
    // C - bit 7,6: modo de operacao - assincrono ('00')
    //   - bit 5,4: modo de paridade - desabilitado ('00')
    //   - bit 3: 1 bits de parada ('0')
    //   - bit 2,1: codigo do numero de bits por frame-usamos 8-'011' ('11')
    //   - bit 0: '0' na transmissao assincrona ('0')
    *ucsr0c = 0x06;
  
    // ativa interrupções globais
  	sei();
  	return;
}

// Rotina de tratamento da interrupcao "recepção completa":
// A interrupção "recepção completa" é disparada sempre que existem 
// dados válidos ainda não lidos no buffer de recepção

// A cada interrupção adicionamos o char recebido ao buffer
ISR(USART_RX_vect){
  	// adiciona o comando recebido ao buffer
	adicionar_buffer(*udr0);	
}

// Rotina de tratamento da interrupcao "transmissao completa":
// A interrupção "transmissão completa" é disparada sempre que o 
// buffer de transmissão foi deslocado e não existem mais dados para envio no UDR0

// A cada interrupção atualizamos o buffer com o char seguinte da mensagem
ISR(USART_TX_vect){
  	// caso a mensagem ainda nao tenha terminado
	if (msg_atual[it] != '\0'){
     	// adcionar o proximo caracter da mensagem no registrador de dados da UART
		*udr0 = msg_atual[it];
      	// incrementar o iterador
    	it++;
    }
    // ao terminar, desabilita a interrupcao de transmissao completa,
  	// para que a uart pare de transmitir o utlimo caracter 
  	else{
    	*ucsr0b &= ~(0x40);
  	}
}

// comando 0: apaga os leds
void apagar_leds(){
  	// desliga os LEDs conectados aos pinos PC3, PC4 e PC5
	*p_portc &= ~(0x38);
   	_delay_ms(500);
}

// comando 3: acende os leds
void acender_leds(){
   	// liga os LEDs conectados aos pinos PC3, PC4 e PC5
	*p_portc |= 0x38;
   	_delay_ms(500);
}

// varredura acesa
void varredura_acesa(){
  // se PC3/PC5 esta ligado, avança/volta para o PC4
  if(LEDatual==3||LEDatual==5){
    LEDultimo = LEDatual; 
    LEDatual = 4;
    // liga PC4
	*p_portc |= 0x10;
    // desliga PC3 e PC5
    *p_portc &= ~(0x28);
  }
  // para PC4, o proximo LED depende do anterior
  else if(LEDatual==4){
    // se o anterior foi PC3, avanca para PC5
    if (LEDultimo==3){
      	LEDatual = 5;
      	// liga PC5
		*p_portc |= 0x20;
    	// desliga PC3 e PC4
        *p_portc &= ~(0x18);
    }
     // se o anterior foi PC5, volta para PC3
    else if (LEDultimo==5){
        LEDatual = 3;
      	// liga PC3
		*p_portc |= 0x08;
      	// desliga PC4 e PC5
        *p_portc &= ~(0x30);
    }
  }
  _delay_ms(500);
}

// varredura apagada
void varredura_apagada(){
  // se PC3/PC5 esta apagado, avança/volta para o PC4
  if(LEDatual==3||LEDatual==5){
    LEDultimo = LEDatual; 
    LEDatual = 4;
    // apaga PC4
	*p_portc &= ~(0x10);
    // liga PC3 e PC5
    *p_portc |= (0x28);
  }
  // para PC4, o proximo LED depende do anterior
  else if(LEDatual==4){
    // se o anterior foi PC3, avanca para PC5
    if (LEDultimo==3){
      	LEDatual = 5;
      	// apaga PC5
		*p_portc &= ~(0x20);
    	// liga PC3 e PC4
        *p_portc |= (0x18);
    }
     // se o anterior foi PC5, volta para PC3
    else if (LEDultimo==5){
        LEDatual = 3;
      	// apaga PC3
		*p_portc &= ~(0x08);
      	// liga PC4 e PC5
        *p_portc |= (0x30);
    }
  }
  _delay_ms(500);
}

// configura a transmissao de uma nova mensagem
void enviar_msg(char* msg){
  	// aponta para a mensagem a ser enviada
  	msg_atual = msg;
  	// insere o primeiro caracter da mensagem no registrador de dados da UART
	*udr0 = msg_atual[0];
  	// atualiza o iterador da mensagem para a posição 1 (seguinte à primeira)
	it = 1;
  	// habilita a interrupcao "transmissao completa"
   	// para que a mensagem comece a ser enviada char a char a cada interrupcao
  	*ucsr0b |= 0x40;
}

int main(){
  	// configura a UART e os pinos de saida
  	setup();
  	// loop principal: processar comandos armazenados no buffer
  	while(1){
      // Delay para melhorar o comportamento da simulacao do Tinkercad
      _delay_ms(1);
      // Se o buffer nao esta vazio, atualiza o estado
      if (ocup_buf > 0){
      	ultimo = estado;
      	estado = remover_buffer();
        vazio=0;
      }
      // Caso o buffer esteja vazio, enviar a mensagem "vazio"
      else{
        estado = ultimo;
      	enviar_msg(msg_v);
        vazio=1;
      }
      // Executa o comando atualizado (novo ou ultimo)
      switch (estado){
      	case '0':
        	// caso o comando nao seja uma repeticao automatica por buffer vazio
        	if (!vazio)
      			enviar_msg(msg_0);
        	apagar_leds();
        	ultimo = estado;
            break;
        case '1':
        	// caso o comando nao seja uma repeticao automatica por buffer vazio
        	if (!vazio)
      			enviar_msg(msg_1);
        	// caso o comando seja novo
        	if (estado!=ultimo){
              	LEDatual = 4;
        		LEDultimo = 5;
        	}
        	varredura_acesa();
        	ultimo = estado;
            break;
        case '2':
        	// caso o comando nao seja uma repeticao automatica por buffer vazio
        	if (!vazio)
      			enviar_msg(msg_2);
        	// caso o comando seja novo
        	if (estado!=ultimo){
              	LEDatual = 4;
        		LEDultimo = 5;
        	}
        	varredura_apagada();
        	ultimo = estado;
            break;
        case '3':
        	if (!vazio)
        		enviar_msg(msg_3);
            acender_leds();
        	ultimo = estado;
            break;
        // codigo incorreto
        default:
        	estado = ultimo;
        	enviar_msg(msg_e);
        	_delay_ms(500);
        	break;
      }
    }
	return 0;
}
