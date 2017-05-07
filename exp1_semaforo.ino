/*EXPERIMENTO 1 - SEMÁFORO - RODRIGO E GUSTAVO. 

O SEMÁFORO CONSISTE EM UM FAROL PARA PEDESTRES E UM PARA CARROS, PARA O PEDESTRE ATRAVESSAR DEVE-SE APERTAR O BOTÃO. O PROGRAMA INICIA VERMELHO PARA OS PEDESTRES E VERDE PARA OS CARROS.CASO O
PEDESTRE PRESSIONE O BOTÃO, AGUARDA-SE 3S E O SEMAFORO DOS CARROS MUDA PARA AMARELO POR 3S, APOS ISSO FICA VERMELHO PARA OS CARROS E VERDE PARA OS PEDESTRES POR 10S, NA SEQUENCIA O SEMAFORO
DOS PEDESTRES PISCA 3X (6S) AVISANDO QUE O SEMAFORO PARA PEDESTRES FICARA VERMELHO, APOS ISSO O SEMAFORO PARA PEDESTRES É FECHADO E ABERTO PARA OS CARROS, VOLTANDO AO ESTADO INICIAL FOI
IMPLEMENTADA UMA MAQUINA DE ESTADO ATRELADA A UM CONTADOR DE 0 A 23S QUE É ACIONADO POR UM BOTÃO( INTERRUPÇÃO DO BOTAO), UMA INTERRUPÇÃO A CADA 1S É CHAMADA PARA INCREMENTAR 1S NO CONTADOR.
FOI IMPLEMENTADO UM SENSOR DE DIA E NOITE, CASO SEJA NOITE O SEMAFORO É DESLIGADO E O LED AMARELO PERMANECE PISCADO ATÉ AMANHECER. SUA LÓGICA É BASEADA EM UM LDR QUE SEU VALOR É CONVERTIDO POR
UM A/D.*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <TimerOne.h>

////////////////////////////////////////////////////////DECLARAÇÃO DE VARIAVEIS///////////////////////////////////////////////////
// VARIAVEIS DOS LEDS DO SEMAFORO
int verm_carro = 8;   //PIN DIGITAL 8
int am_carro = 9;     //PIN 9
int verde_carro = 10; //PIN 10
int verm_ped = 11;    //PIN 11
int verde_ped = 12;   //PIN 12

//VARIAVEIS PARA BOTAO
int button = 2; // PIN DIGITAL 2
int buttonState = 0;

//CONTADOR
int CONT_botao_ped = 0;

//flag para piscar semaforo dos pedestres 
int PISCAR = 0;

//A/D
int ldrPin = 0; //LDR no pino analígico A1
int ldrValor = 0; //Valor lido do LDR
int toggle = 0; // TOGGLE PARRA AMARELO.
int noite = 0; // FLAG PARA SINALIZAR NOITE

///////////////////////////////////////////////////INTERRUPÇÕES//////////////////////////////////////////////////////////////

// INTERRUPÇÃO DE TEMPO
void ISR_timer() {
 
  if(buttonState == HIGH){  //SE O BOTAO DE PEDESTRE FOI PRESSIONADO INICIA O CONTADOR QUE IRÁ INCREMENTAR A CADA 1s
  	CONT_botao_ped++;
  }
  
  if(PISCAR){  // CASO O SEMAFORO ESTEJA NO ESTADO PARA PISCAR LEDS DO PEDESTRE.
    toggle = !toggle; // TOGGLE PARA ALTERAR ESTADO DOS LEDS
  }

  ldrValor = analogRead(ldrPin); //LEITURA E CONVERSAO DO VALOR DO LDR.
  if (ldrValor >= 1000){ // CASO SEJA NOITE.
    noite = 1; //SETA FLAG PARA DESLIGAR SEMAFORO.
  }  
  else noite = 0; // CASO CONTRARIO, ESTA DE DIA.
}

// INTERRUPÇÃO DO BOTAO DOS PEDESTRES.
  void interrupcao(){ 
      buttonState = 1; //FLAG PARA INICIAR CONTADOR.
} 

/////////////////////////////////////////////////SETUP/////////////////////////////////////////////////////////////////

void setup() {
  pinMode(verm_carro, OUTPUT);
  pinMode(am_carro, OUTPUT);
  pinMode(verde_carro, OUTPUT);
  pinMode(verm_ped, OUTPUT);
  pinMode(verde_carro, OUTPUT);   
  pinMode(button, INPUT);
 
  attachInterrupt(0,interrupcao,RISING); //Configurando a interrupção para botão, borda de descida.

// Inicializar a rotina de interrupcao periodica
  Timer1.initialize(1000000); // Interrupcao a cada 1s
  Timer1.attachInterrupt(ISR_timer); // Associa a interrupcao periodica a funcao ISR_timer

  Serial.begin(9600); //Inicia a comunicação serial
}

void loop() {

//ESTADO INICIAL, ABERTO PARA CARROS, FECHADO PARA PEDESTRES E DE DIA.
  if((CONT_botao_ped == 0) && (noite == LOW)){
        digitalWrite(verm_carro, LOW); // Vermelho p carros apagado
        digitalWrite(am_carro, LOW); // amarelo p carros apagado
        digitalWrite(verde_carro, HIGH); // verde p carros
        digitalWrite(verm_ped, HIGH); // Vermelho p pedestres
        digitalWrite(verde_ped, LOW); // verde p pedestres apagado
  }
  
// SE O BOTAO FOI PRESSIONADO,AGUARDA 3S, EM SEGUIDA SETA AMARELO PARA OS CARROS E APAGAR O VERDE DOS CARROS
  if(CONT_botao_ped == 3){
        digitalWrite(am_carro, HIGH); // amarelo p carros
        digitalWrite(verde_carro, LOW); // verde p carros apagado
  }  
  
//APOS 3s NO AMARELO, SETA VERMELHO PARA CARROS E VERDE PARA PEDESTRES POR 10s.
  if(CONT_botao_ped == 6){
    digitalWrite(verm_ped, LOW); // Vermelho pedestres apagado
    digitalWrite(verde_ped, HIGH); // Verde p pedestres
    digitalWrite(verm_carro, HIGH); // Vermelho p carros
    digitalWrite(am_carro, LOW); // apaga amarelo
  }
 
 //APOS ABERTO 10S PARA OS PEDESTRE, PISCA SEMAFORO DOS PEDESTRE AVISANDO QUE IRÁ FECHAR. 
  while((CONT_botao_ped >= 16) && (CONT_botao_ped <= 22)){  // CASO O SEMAFORO ESTEJA NO ESTADO PARA PISCAR LEDS DO PEDESTRE.
    PISCAR = 1;
    digitalWrite(verde_ped, toggle); // Verde dos pedestres PISCA
    digitalWrite(verm_ped, toggle); // Vermelho dos psedestres PISCA
  }
  PISCAR = 0;
  
//APOS 10s E DEPOIS DE PISCAR 3x O SEMAFORO DOS PEDESTRES, ABRE PARA CARROS E FECHA PARA PEDESTRES.
  if(CONT_botao_ped == 23){

    digitalWrite(verde_ped, LOW); // Verde p pedestres apagado
    digitalWrite(verm_ped, HIGH); // vermelho p pedestres
    digitalWrite(verm_carro, LOW); // vermelho p carros apagado
    digitalWrite(verde_carro, HIGH); // Verde p carros
    
    CONT_botao_ped = 0; //zera contagem do semaforo
    buttonState = 0; //reset do botão
  }
  
//SE ESTIVER DE NOITE, DESLIGA O SEMAFORO E PISCA O AMARELO.
  if(noite){
    CONT_botao_ped = 0; // IMPEDE ENTRAR NOS ESTADOS DENTRO DO LOOP
    
    toggle = !toggle;
    digitalWrite(am_carro, toggle); // PISCA AMARELO
    digitalWrite(verm_carro, LOW); // APAGA VERMELHO CARROS
    digitalWrite(verde_carro, LOW); // APAGA VERDE CARROS
    digitalWrite(verde_ped, LOW); // APAGA VERDE PEDESTRE
    digitalWrite(verm_ped, LOW); // apaga vermelho pedestres
  }    
}
