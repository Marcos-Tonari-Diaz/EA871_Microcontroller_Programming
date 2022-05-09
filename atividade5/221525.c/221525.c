#include <util/delay.h>

// Strings de mensagens
char msg_np[] = "Transmissao serial utilizando a USART: verificacao do termino da transmissao por varredura. O botao nao esta pressionado.\n\n";

char msg_p[] = "Transmissao serial utilizando a USART: verificacao do termino da transmissao por varredura. O botao esta pressionado.\n\n";

// Ponteiros globais para os registradores das portas e da USART

//PORTD
unsigned char *p_portb;
unsigned char *p_ddrb;
unsigned char *p_pinb;

//USART
unsigned char *ubrr0h;
unsigned char *ubrr0l;
unsigned char *ucsr0a;
unsigned char *ucsr0b;
unsigned char *ucsr0c;
unsigned char *udr0;

// realiza o setup de pinos utilizando 
// os registradores das portas
void setup(void){
  
    // cria referencias para os registradores
    
    // Push_Button
    // PORTB
    p_portb = (unsigned char*) 0x25;
    p_ddrb = (unsigned char*) 0x24;
    p_pinb = (unsigned char*) 0x23;
    
    // PB1: configurar como entrada
    // input: setar o bit 1 do DDRD como 0 (e nao mexer nos outros)
    *p_ddrb &= ~(0x02); 
    // ativar pull-up resistor: setar o bit0 do Data Register PORTD com 1 (e nao mexer nos outros)
    *p_portb |= 0x02;

    // USART
    ubrr0h = (unsigned char*) 0xC5;    
    ubrr0l = (unsigned char*) 0xC4;
    ucsr0a = (unsigned char*) 0xC0;
    ucsr0b = (unsigned char*) 0xC1;
    ucsr0c = (unsigned char*) 0xC2;
    udr0 = (unsigned char*) 0xC6;

    // Baud Rate Registers para 115.2K bps
    *ubrr0h = 0x00;
    *ubrr0l = 0x08;

    // USART Control Status Registers
    // A- bit 1: modo de velocidade - normal ('1')
    //  - bit 0: desabilita a comunicação multi-processador ('1')
    *ucsr0a &= ~(0x03);
    // B- bit 7, 6, 5: desabilitar interrupcoes por USART ('000')
    //  - bit 4, 3: desabilitar o receptor e habilitar o transmissor ('01')
    //  - bit 2: codigo do numero de bits por frame-usamos 8-'011' ('0')
    //  - bits 1, 0: usado para frames com 9bits (ignorar)
    //setar o 1
    *ucsr0b |= 0x08;
    //setar os 0s
    *ucsr0b &= 0x0B;
    // C - bit 7,6: modo de operacao - assincrono ('00')
    //   - bit 5,4: modo de paridade - habilitado par ('10')
    //   - bit 3: 2 bits de paridade ('1')
    //   - bit 2,1: codigo do numero de bits por frame-usamos 8-'011' ('11')
    //   - bit 0: '0' na transmissao assincrona ('0')
    *ucsr0c = 0x2E;
    
    return;
}

int main(){
  setup();
  // amostras do estado do push-button (inicialmente aberto)
  unsigned char button_sample = 0x02;
  // aponta para a mensagem a ser enviada
  char* msg_to_send = msg_np;
  // variaveis para iterar a string
  int i=0;
  char c='\n';
  
  while(1){
    //1. verificar o botao
    // leitura do push-button
    button_sample = ((*p_pinb & 0x02)>>1);
    
    //2. enviar a mensagem
    // botao pressionado
    if(!button_sample) {msg_to_send = msg_p;}
    // botao aberto
    else {msg_to_send = msg_np;}
    // itera a string byte a byte
    while(c!='\0'){
      c = msg_to_send[i];
      i++;
      // insere o byte no registrador de dados UDR0
      *udr0 = c;
      //aguardar a transmissao do byte
      // buffer de transmissao vazio - bit UDRE0 = 1 (bit 5)
      while(!((*ucsr0a & 0x20)>>5)){}
    }
    // resetar as variaveis que controlam a iteracao
    i=0;
    c='\n';
    
    //3. aguardar 500ms ate o envio da proxima mensagem
    _delay_ms(500); 
  }
  return 0;
}
