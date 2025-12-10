#include <Wire.h>
#include <VL53L0X.h>
#include <HCSR04.h>
#include <LinkedList.h>

// ======================================================================
// SENSOR ULTRASSÔNICO
// ======================================================================
UltraSonicDistanceSensor usSensor(4, 5);

double us_distAnterior = 0;
double us_distAtual = 0;
double us_distInicial = 0.5;

unsigned long us_tInicial = 0;
unsigned long us_tFinal = 0;
unsigned long us_tAnterior = 0;

bool us_inicio = true;
bool us_parou = true;
bool us_mov = false;

double us_velInst = 0;
double us_velInstAnterior = 0;
bool us_temVelAnterior = false;
double us_dt = 0;

unsigned long agora = 0;

// listas temporárias
LinkedList<double> us_velocidadesInst;
LinkedList<double> us_aceleracoesInst;


// ======================================================================
// SENSOR ÓPTICO VL53L0X
// ======================================================================
VL53L0X sensor;

double op_distAnterior = 0;
double op_distAtual = 0;
double op_distInicial = 0.4;

unsigned long op_tInicial = 0;
unsigned long op_tFinal = 0;
unsigned long op_tAnterior = 0;

bool op_inicio = true;
bool op_parou = true;
bool op_mov = false;

double op_velInst = 0;
double op_velInstAnterior = 0;
bool op_temVelAnterior = false;
double op_dt = 0;

uint16_t op_dist_mm = 0;

// listas temporárias
LinkedList<double> op_velocidadesInst;
LinkedList<double> op_aceleracoesInst;


// ======================================================================
// STRUCT COM REGISTRO FINAL
// ======================================================================
struct RegistroMovimento {
  double tempoTotal;
  double distTotal;
  LinkedList<double> velocidadesInst;
  LinkedList<double> aceleracoesInst;
};

LinkedList<RegistroMovimento> lista_US;
LinkedList<RegistroMovimento> lista_OP;

const uint8_t MAX_REG = 10;


// ======================================================================
// IMPRIMIR LISTAS
// ======================================================================
void imprimirRegistrosUS() {
  Serial1.println("#LISTA_US");

  for (int i = 0; i < lista_US.size(); i++) {
    RegistroMovimento r = lista_US.get(i);

    Serial1.println("REG:");
    Serial1.print("tempo="); Serial1.println(r.tempoTotal, 6);
    Serial1.print("dist=");  Serial1.println(r.distTotal, 6);

    Serial1.println("velocidades:");
    for (int v = 0; v < r.velocidadesInst.size(); v++)
      Serial1.println(r.velocidadesInst.get(v), 6);

    Serial1.println("aceleracoes:");
    for (int a = 0; a < r.aceleracoesInst.size(); a++)
      Serial1.println(r.aceleracoesInst.get(a), 6);
  }

  Serial1.println("#FIM_US");
}

void imprimirRegistrosOP() {
  Serial.println("#LISTA_OP");

  for (int i = 0; i < lista_OP.size(); i++) {
    RegistroMovimento r = lista_OP.get(i);

    Serial.println("REG:");
    Serial.print("tempo="); Serial.println(r.tempoTotal, 6);
    Serial.print("dist=");  Serial.println(r.distTotal, 6);

    Serial.println("velocidades:");
    for (int v = 0; v < r.velocidadesInst.size(); v++)
      Serial.println(r.velocidadesInst.get(v), 6);

    Serial.println("aceleracoes:");
    for (int a = 0; a < r.aceleracoesInst.size(); a++)
      Serial.println(r.aceleracoesInst.get(a), 6);
  }

  Serial.println("#FIM_OP");
}


// ======================================================================
// SETUP
// ======================================================================
void setup() {
  Serial.begin(115200);    
  Serial1.begin(115200);   

  Wire.begin();
  sensor.setTimeout(500);

  if (!sensor.init()) {
    Serial.println("#ERRO_SENSOR_OPTICO");
  } else {
    sensor.setMeasurementTimingBudget(200000);
    Serial.println("#OP_OK");
  }

  Serial1.println("#US_OK");
}


// ======================================================================
// LOOP
// ======================================================================
void loop() {

  agora = millis();   

  // ===============================================================
  // SENSOR ULTRASSÔNICO
  // ===============================================================
  us_distAnterior = us_distAtual;
  us_distAtual = usSensor.measureDistanceCm() / 100.0;

  if (us_mov) {
    if (us_tAnterior != 0) {
      us_dt = (agora - us_tAnterior) / 1000.0;

      if (us_dt > 0) {
        us_velInst = -(us_distAtual - us_distAnterior) / us_dt;

        double aceleracaoInst = 0;
        if (us_temVelAnterior)
          aceleracaoInst = (us_velInst - us_velInstAnterior) / us_dt;

        us_velInstAnterior = us_velInst;
        us_temVelAnterior = true;

        us_velocidadesInst.add(us_velInst);
        us_aceleracoesInst.add(aceleracaoInst);
      }
    }
  }

  us_tAnterior = agora;

  if (us_distAnterior >= us_distInicial && us_distAtual < us_distInicial && us_inicio) {
    us_tInicial = agora;

    us_inicio = false;
    us_parou = false;
    us_mov = true;

    us_velocidadesInst.clear();
    us_aceleracoesInst.clear();
    us_temVelAnterior = false;
  }

  if (!us_inicio && !us_parou && us_distAtual < 0.1) {
    us_tFinal = agora;
    double tempoTotal = (us_tFinal - us_tInicial) / 1000.0;
    double distTotal = us_distAtual - us_distInicial;

    RegistroMovimento reg;
    reg.tempoTotal = tempoTotal;
    reg.distTotal  = distTotal;
    reg.velocidadesInst = us_velocidadesInst;
    reg.aceleracoesInst = us_aceleracoesInst;

    if (lista_US.size() >= MAX_REG)
      lista_US.remove(0);

    lista_US.add(reg);

    Serial1.println("#US_MOV_FINALIZADO");

    us_parou = true;
    us_mov = false;
  }

  // ===============================================================
  // SENSOR ÓPTICO
  // ===============================================================
  op_distAnterior = op_distAtual;

  op_dist_mm = sensor.readRangeSingleMillimeters();
  if (!sensor.timeoutOccurred() && op_dist_mm > 0 && op_dist_mm < 4000) 
    op_distAtual = op_dist_mm / 1000.0;

  if (op_mov) {
    if (op_tAnterior != 0) {
      op_dt = (agora - op_tAnterior) / 1000.0;

      if (op_dt > 0) {
        op_velInst = -(op_distAtual - op_distAnterior) / op_dt;

        double aceleracaoInst = 0;
        if (op_temVelAnterior)
          aceleracaoInst = (op_velInst - op_velInstAnterior) / op_dt;

        op_velInstAnterior = op_velInst;
        op_temVelAnterior = true;

        op_velocidadesInst.add(op_velInst);
        op_aceleracoesInst.add(aceleracaoInst);
      }
    }
  }

  op_tAnterior = agora;

  if (op_distAnterior >= op_distInicial && op_distAtual < op_distInicial && op_inicio) {
    op_tInicial = agora;

    op_inicio = false;
    op_parou = false;
    op_mov = true;

    op_velocidadesInst.clear();
    op_aceleracoesInst.clear();
    op_temVelAnterior = false;
  }

  if (!op_inicio && !op_parou && op_distAtual < 0.1) {
    op_tFinal = agora;
    double tempoTotal = (op_tFinal - op_tInicial) / 1000.0;
    double distTotal = op_distAtual - op_distInicial;

    RegistroMovimento reg;
    reg.tempoTotal = tempoTotal;
    reg.distTotal  = distTotal;
    reg.velocidadesInst = op_velocidadesInst;
    reg.aceleracoesInst = op_aceleracoesInst;

    if (lista_OP.size() >= MAX_REG)
      lista_OP.remove(0);

    lista_OP.add(reg);

    Serial.println("#OP_MOV_FINALIZADO");

    op_parou = true;
    op_mov = false;
  }

  // ===============================================================
  // COMANDO PARA LISTA
  // ===============================================================
  //if (Serial.available()) {
    //if (Serial.read() == 'L')
      //imprimirRegistrosOP();
  //}

  //if (Serial1.available()) {
    //if (Serial1.read() == 'L')
      //imprimirRegistrosUS();
  //}
}
