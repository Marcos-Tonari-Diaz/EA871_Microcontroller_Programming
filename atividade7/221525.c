    
    /*! Justificativa para a as escolhas do pre-scaler e valor de comparação:
     *  Escolhendo 200 como valor de comparação da contagem do timer 
     *  e 8 como multiplicação do período de contagem pelo prescaler
     *  temos: 200*8/16e6 = 0,0001s como periodo da interrupção
     *  Esse período "normalizado" torna as contagens por software, que são discretas,
     *  mais precisas e simples de implementar
     */

/* Bibliotecas */
#include <avr/interrupt.h>

// Ponteiros globais para os registradores das portas e da USART

//PORTD
unsigned char *p_portd;
unsigned char *p_ddrd;

//PORTB
unsigned char *p_portb;
unsigned char *p_ddrb;

//USART
unsigned char *ubrr0h;
unsigned char *ubrr0l;
unsigned char *ucsr0a;
unsigned char *ucsr0b;
unsigned char *ucsr0c;
unsigned char *udr0;

//Temporizador
unsigned char *p_tccr2a;
unsigned char *p_tccr2b;
unsigned char *p_timsk2;
unsigned char *p_ocr2a;
unsigned char *p_ocr2b;


// mensagem
char msg[] = "Atividade 7 - Interrupcoes temporizadas tratam concorrencia entre tarefas! \n\n";

// indica se a mensagem terminou
volatile char mensagem_enviada;

// Iterador da string
volatile unsigned char it;

// contadores dos leds incorporado e externo
int led_inc=0;
int led_ext=0;

// configuracao dos registradores
void setup(){
  
    // desativa interrupções globais para configuração
    cli();
  
    // PORTD
    p_portd = (unsigned char*) 0x2b;
    p_ddrd = (unsigned char*) 0x2a;
  
    // PORTB
    p_portb = (unsigned char*) 0x25;
    p_ddrb = (unsigned char*) 0x24;
  
    // USART
    ubrr0h = (unsigned char*) 0xC5;
    ubrr0l = (unsigned char*) 0xC4;
    ucsr0a = (unsigned char*) 0xC0;
    ucsr0b = (unsigned char*) 0xC1;
    ucsr0c = (unsigned char*) 0xC2;
    udr0 = (unsigned char*) 0xC6;
    
    //Temporizador
    p_tccr2a = (unsigned char*) 0xB0;
    p_tccr2b = (unsigned char*) 0xB1;
    p_timsk2 = (unsigned char*) 0x70;
    p_ocr2a= (unsigned char*) 0xb3;
    p_ocr2b= (unsigned char*) 0xb4;
  
    //Configuração dos registradores:
    
    // PORTD
    // Configurar PD6 como output: setar o bis 6 como 1
    *p_ddrd |= (0x40);

    // PORTB
    // Configurar PB5 como output: setar o bit 5 como 1
    *p_ddrb |= (0x20);
  
    // USART
    // Baud Rate Registers para 19.2K bps
    *ubrr0h = 0x00;
    *ubrr0l = 0x33;

    // USART Control Status Registers
    // A- bit 1: modo de velocidade - normal ('0')
    //  - bit 0: desabilita a comunicação multi-processador ('0')
    *ucsr0a &= ~(0x03);
    // B- bit 7: desabilitar interrupcao de recepcao completea
    //  - bit 6: desabilitar interrupcao de transmissao completa (inicialmente)
    //  - bit 5: desabilitar a interrupcao de registrador de dados vazio
    //  - bit 4: desabilitar o receptor 
    //  - bit 3: habilitar o transmissor 
    //  - bit 2: codigo do numero de bits por frame-usamos 8-'011' ('0')
    //  - bits 1, 0: usado para frames com 9bits (ignorar)
    //setar os 1s
    *ucsr0b |= 0x08;
    //setar os 0s
    *ucsr0b &= ~(0xF4);
    // C - bit 7,6: modo de operacao - assincrono ('00')
    //   - bit 5,4: modo de paridade - desabilitado ('00')
    //   - bit 3: 1 bits de parada ('0')
    //   - bit 2,1: codigo do numero de bits por frame-usamos 8-'011' ('11')
    //   - bit 0: '0' na transmissao assincrona ('0')
    *ucsr0c = 0x06;
  
    // Temporizador
    // Output Compare Register A
    // uma interrupção é disparada quando o contador chega ao valor desse registrador
    *p_ocr2a = 200;
    // Output Compare Register B
    // uma interrupção é disparada quando o contador chega ao valor desse registrador
    *p_ocr2b = 200;
    // Timer/Counter2 Interrupt Mask Register
    // - bits 7 a 3: reservados, sempre 0 ('00000')
    // - bit 2: habilitar interrupcao de comparação com Output A ('1')
    // - bit 1: habilitar interrupcao de comparação com Output B ('1')
    // - bit 0: desabilitar interrução de overflow ('0')
    *p_timsk2 = 6;
    // Timer/Counter Control Register A
    // - bits 7 e 6: desconectar OC0A ('00')
    // - bits 5 e 4: desconectar OC0B ('00')
    // - bits 3 e 2: reservados, sempre 0 ('00')
    // - bits 0 e 1: modo de operacao CTC ('10')
    *p_tccr2a = 2;
    // Timer/Counter Control Register B
    // - bits 7 e 6: não forçar comparação ('00')
    // - bits 5 e 4: reservados, sempre 0 ('00')
    // - bit 3: modo de operacao CTC  ('0')
    // - bits 0, 1 e 2: configuração do prescaler: 
    // dividir a frequência por 8  ('010')
    *p_tccr2b = 2;
     
    // ativa interrupções globais
    sei();
    return;
}

// Rotina de tratamento da interrupcao "transmissao completa":
// A interrupção "transmissão completa" é disparada sempre que o 
// buffer de transmissão foi deslocado e não existem mais dados para envio no UDR0

// A cada interrupção atualizamos o buffer com o char seguinte da mensagem
ISR(USART_TX_vect){
    // caso a mensagem ainda nao tenha terminado
  if (msg[it] != '\0'){
      // adcionar o proximo caracter da mensagem no registrador de dados da UART
    *udr0 = msg[it];
        // incrementar o iterador
      it++;
    }
    // ao terminar, desabilita a interrupcao de transmissao completa,
    // para que a uart pare de transmitir o utlimo caracter 
    else{
      *ucsr0b &= ~(0x40);
        mensagem_enviada = 1;
    }
}

void envia_mensagem(){
  // insere o primeiro caracter da mensagem no registrador de dados da UART
  *udr0 = msg[0];
    // atualiza o iterador da mensagem para a posição 1 (seguinte à primeira)
  it = 1;
    // habilita a interrupcao "transmissao completa"
    // para que a mensagem comece a ser enviada char a char a cada interrupcao
    *ucsr0b |= 0x40;
    // espera a mensagem ser enviada
    while(!mensagem_enviada);
    // após o termino do envio da mensagem, esperar 5s
    _delay_ms(5000);
    // resetar a variavel de controle
    mensagem_enviada = 0;
}

// led incorporado
// Tratamento da interrupcao compare match A:
ISR(TIMER2_COMPA_vect){
  led_inc++; // incrementa a varivel
  if (led_inc==5000){ // caso a varivavel tenha contado um periodo
    *p_portb ^= 0x20; // toggle na saida do PB5
    led_inc=0; // reinicia a contagem de periodo
  }
}

// led externo
// Tratamento da interrupcao compare match B:
ISR(TIMER2_COMPB_vect){
  led_ext++; // incrementa a varivel
  if (led_ext==7800){ // caso a varivavel tenha contado um periodo
    *p_portd ^= 0x40; // toggle na saida do PD6
    led_ext=0; // reinicia a contagem de periodo
  }
}

int main(){
    setup(); // configura os registradores
    mensagem_enviada = 0; // inicia sem ter enviado a mensagem
    while(1){
      _delay_ms(1); // Delay para melhorar o comportamento da simulacao do Tinkercad
      envia_mensagem();
    }
    return 0;
}
