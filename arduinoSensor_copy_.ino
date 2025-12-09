#define RES 1

//incializaçao
int ponteH = 9;  // MOS Module
int sensor = A10;
int potenciometro = A8;

// Controle de tempo
unsigned long inicio = 0;
unsigned long inicioF = 0;
float i = 0;  //corrente

// Variáveis desligadas
//-->int valorPotenciometro = 0;

void setup() {
  Serial.begin(9600);

  pinMode(ponteH, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);


  inicio = millis();
  inicioF = millis();
}


int lerPotenciometro() {
  int valorLido = analogRead(potenciometro);

  //-->Trecho removido do if
  //(nivel != valorPotenciometro && (nivel - valorPotenciometro > 1 || nivel - valorPotenciometro < -1)) ||
  //-->Somente de 2 em 2s
  if (((millis() - inicioF) / 1e3) > 2.0) {
    inicioF = millis();

    //-->Nivel do motor (0-100%)
    //int nivel = map(valorLido, 0, 1023, 0, 100);
    //Serial.print("Capacidade: ");
    //Serial.print(nivel);
    //Serial.print("% | Sensor: ");
    //valorPotenciometro = nivel;

    double t = (millis() - inicio) / 1e3;
    float sensorValue = analogRead(sensor) * (5.0 / 1023.0);
    float campo = ((sensorValue - 2.5) / 0.005) * 1e-4;  //Gauss->tesla = 1e-4
    float V = analogRead(A0) * (5.0 / 1023.0);           // tensão em volts
    float V1 = analogRead(A1) * (5.0 / 1023.0);

    i = (V1 - V) / RES;

    Serial.print(t);
    Serial.print(",");
    Serial.print(i);
    Serial.print(",");
    Serial.print(V1);
    Serial.print(",");
    Serial.println(sensorValue);
  }

  int motor = map(valorLido, 0, 1023, 0, 255);
  analogWrite(ponteH, motor);
}


void loop() {
  lerPotenciometro();
}
