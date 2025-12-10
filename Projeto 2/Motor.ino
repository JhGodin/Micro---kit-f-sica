#define PPR 20
#define RAI 0.012  //metros
#define RES 1
#include <GFButton.h>

//inicializaçao
int ponteH = 9;     // MOS Module
int potenciometro = A5;  //Potenciometro
int encoder = 3;      // Pino de entrada do encoder

// Estados do encoder
int estadoAnterior = 0;

// Variáveis principais
unsigned int indice = 0;     //reseta a cada PPR
float i = 0;                 //corrente

// Controle de tempo
unsigned long inicio = 0;
unsigned long inicioE = 0;
unsigned long inicioC = 0;

unsigned long ultimaRotacao = 0;



//Memoria temporaria 20ticks
typedef struct lista Lista;
struct lista {
  float vAng;  //rad/s
  float v;     //m/s
  float tAng;  //s
};
Lista lista[PPR];

//variaveis "desligadas"
//-->unsigned long ultimaCorrente = 0;  //Periodo da ultima medicao
//-->int valorPotenciometro = 0;        //valor lido de 0 a 100
//-->unsigned long contador = 0;        //conta cada tick
//-->unsigned long rotacao = 0;         //soma 1 a cada PPR

bool lerEncoder() {
  int estadoAtual = digitalRead(encoder);
  if (estadoAnterior != estadoAtual) {
    indice++;

    //-->Pega o Tempo entre ticks calcula a Va e Vm e salma tudo numa listaEncadeada
    //double t = ((micros() - inicioE) / 1e6);  // tempo entre pulsos em segundos
    //inicioE = micros();
    //
    //float vAngular = calculaVelocidadeAngular(t);
    //float v = calculaVelocidade(vAngular);
    //insere(vAngular, v, t);
    //contador++;
  }
  estadoAnterior = estadoAtual;
  return;
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(ponteH, OUTPUT);
  pinMode(encoder, INPUT_PULLUP);

  estadoAnterior = digitalRead(encoder);

  int in = digitalPinToInterrupt(encoder);
  attachInterrupt(in, lerEncoder, CHANGE);

  inicioE = micros();
  inicioC = millis();
}




int lerPotenciometro() {
  int valorLido = analogRead(potenciometro);
  int motor = map(valorLido, 0, 1023, 0, 255);
  analogWrite(ponteH, motor);

  int A = analogRead(A0);
  int A1 = analogRead(A1);

  A = A * (5.0 / 1023.0);    // tensão em volts
  A1 = A1 * (5.0 / 1023.0);  // tensão em volts

  i = (A1 - A) / RES;

  //-->Apresenta o valor do Potenciometro
  //
  //  int nivel = map(valorLido, 0, 1023, 0, 100);
  //  if ((nivel != valorPotenciometro && (nivel - valorPotenciometro > 1 || nivel - valorPotenciometro < -1) || ((millis() - inicioC) / 1e3) > 2.0)) {
  //    inicioC = millis();
  //
  //    //Serial.print("Corrente: ");
  //    //Serial.print(i);
  //    //Serial.print(" A | Tempo: ");
  //
  //    //double t = ((millis() - ultimaCorrente) / 1e3);       //-->tempo entre a variação da ultima corrente
  //    //ultimaCorrente=millis();
  //    //Serial.print(t);
  //
  //    valorPotenciometro = nivel;
  //  }
}
double calculaVelocidadeAngular(double t) {
  return (2 * PI) / (PPR * t);  // ω = 2π / (PPR * t);
}
double calculaVelocidade(double vAngular) {
  return RAI * vAngular;
}
void insere(float a, float x, float t) {
  lista[indice].vAng = a;
  lista[indice].v = x;
  lista[indice].tAng = t;
}
void frotacao() {
  if (indice >= 20) {
    float nowRotacao = ((millis() - ultimaRotacao) / 1e3);  //tempo de rotação
    float va = ((2 * PI) / nowRotacao);                     //velocidade angular da rotação (rad/s)
    //float v = RAI * va;                                   //velocidade da rotação (m/s)

    Serial.print(nowRotacao);  //tempoMedio    
    Serial.print(",");
    Serial.print(va);  //veloAngularMedia    
    Serial.print(",");
    Serial.println(i);

    indice = 0;
    ultimaRotacao = millis();


    //float soma1 = 0;                //-->Vam do tick
    //float soma2 = 0;                //-->Vm do tick
    //double temp3 = 0;               //-->Tm do tick
    //rotacao++;                      //-->Contador

    //for (int i = 0; i < PPR; i++) {
    //  soma1 += lista[i].vAng;
    //  soma2 += lista[i].v;
    //  temp3 += lista[i].tAng;
    //}

    //soma1 = (soma1 / 20);
    //soma2 = (soma2 / 20);
    //temp3 = (temp3 / 20);

    //Serial.println();
    //Serial.print("Tick --> Velocidade: ");
    //Serial.print(soma1);  //veloAngularMedia
    //Serial.print(" rad/s | ");
    //Serial.print(soma2);  //velocidadeMedia
    //Serial.print(" m/s | Tempo: ");
    //Serial.print(temp3);  //tempoMedio
    //Serial.println(" s");
  }
}




void loop() {
  frotacao();
  lerPotenciometro();
}
