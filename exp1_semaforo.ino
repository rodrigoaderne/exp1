#include <TimerOne.h>
// VARIAVEIS DOS LEDS DO SEMAFORO
int verm_carro = 8;
int am_carro = 9;
int verde_carro = 10;
int verm_ped = 11;
int verde_ped = 12;

//VARIAVEIS PARA BOTAO
int button = 2;
int buttonState = 0;

//CONTADOR
int CONT_botao_ped = 0;


//A/D
int ldrPin = 0; //LDR no pino analígico 8
int ldrValor = 0; //Valor lido do LDR

int toggle = 0; // TOGGLE PARRA AMARELO.
int noite = 0; // FLAG PARA SINALIZAR NOITE

void ISR_timer() {
  if(buttonState == HIGH){  //SE O BOTAO DE PEDESTRE FOI PRESSIONADO INICIA O CONTADOR QUE IRÁ INCREMENTAR A CADA 1s
  	CONT_botao_ped++;
  }
  
  if((CONT_botao_ped >= 16) && (CONT_botao_ped <= 22)){  // CASO O SEMAFORO ESTEJA NO ESTADO PARA PISCAR LEDS DO PEDESTRE.
    toggle = !toggle;
    digitalWrite(verde_ped, toggle); // Verde dos pedestres PISCA
    digitalWrite(verm_ped, toggle); // Vermelho dos psedestres PISCA
}

  ldrValor = analogRead(ldrPin); //LEITURA E CONVERSAO DO VALOR DO LDR.
  if (ldrValor >= 1000){ // CASO SEJA NOITE.
    noite = 1;
    CONT_botao_ped = 0; // IMPEDE ENTRAR NOS ESTADOS DENTRO DO LOOP
    
    toggle = !toggle;
    digitalWrite(am_carro, toggle); // PISCA AMARELO
    digitalWrite(verm_carro, LOW); // APAGA VERMELHO CARROS
    digitalWrite(verde_carro, LOW); // APAGA VERDE CARROS
    digitalWrite(verde_ped, LOW); // APAGA VERDE PEDESTRE
    digitalWrite(verm_ped, LOW); // apaga vermelho pedestres
  }  
  else noite = 0; // CASO CONTRARIO, ESTA DE DIA.
}

void interrupcao(){ // INTERRUPÇÃO DO BOTAO DOS PEDESTRES.
      buttonState = 1;
} 


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
  
  // SE O BOTAO FOI PRESSIONADO, 3s APOS O SEMAFORO DE CARROS FICA AMARELO POR 3s
  if(CONT_botao_ped == 3){
        digitalWrite(am_carro, HIGH); // amarelo p carros
        digitalWrite(verde_carro, LOW); // verde p carros apagado
  }  
  
  //APOS 3s, VERMELHO PARA CARROS E VERDE PARA PEDESTRES POR 10s.
  if(CONT_botao_ped == 6){
    digitalWrite(verm_ped, LOW); // Vermelho pedestres apagado
    digitalWrite(verde_ped, HIGH); // Verde p pedestres
    digitalWrite(verm_carro, HIGH); // Vermelho p carros
    digitalWrite(am_carro, LOW); // apaga amarelo
  }
  
  //APOS 10s E DEPOIS DE PISCAR 2x O SEMAFORO DOS PEDESTRES (INTERRRUPÇÃO ISR_timer), ABRE PARA CARROS E FECHA PARA PEDESTRES
  if(CONT_botao_ped == 23){

    digitalWrite(verde_ped, LOW); // Verde p pedestres apagado
    digitalWrite(verm_ped, HIGH); // vermelho p pedestres
    digitalWrite(verm_carro, LOW); // vermelho p carros apagado
    digitalWrite(verde_carro, HIGH); // Verde p carros
  
    CONT_botao_ped = 0; //zera contagem do semaforo
    buttonState = 0; //reset do botão

  }
}
