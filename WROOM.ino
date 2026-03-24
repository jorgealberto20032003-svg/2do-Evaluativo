#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(2, 5); 
const byte address[6] = "12321";

// Pines para LEDs de Monitoreo Físico
const int ledBomba = 25;  // Azul
const int ledFoco = 26;   // Rojo
const int ledVent  = 27;  // Verde

struct Payload {
  int humedadSuelo;
  float temperatura;
  bool bombaOn;
  bool focoOn;
  bool ventOn;
};

Payload reporte;

void setup() {
  Serial.begin(115200);
  Serial.println("\n======================================");
  Serial.println("   ESTACION DE MONITOREO INVERNADERO  ");
  Serial.println("======================================\n");
  
  pinMode(ledBomba, OUTPUT);
  pinMode(ledFoco, OUTPUT);
  pinMode(ledVent, OUTPUT);

  SPI.begin(18, 19, 23, 5);
  if (!radio.begin()) {
    Serial.println("CRITICO: Fallo en el modulo de Radio");
    while(1);
  }

  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&reporte, sizeof(Payload));

    // 1. Actualizar LEDs físicos en el WROOM
    digitalWrite(ledBomba, reporte.bombaOn ? HIGH : LOW);
    digitalWrite(ledFoco,  reporte.focoOn  ? HIGH : LOW);
    digitalWrite(ledVent,  reporte.ventOn  ? HIGH : LOW);

    // 2. Reporte Detallado en el Serial Monitor
    Serial.println("----- NUEVO REPORTE RECIBIDO -----");
    Serial.print("LECTURAS -> Suelo: "); Serial.print(reporte.humedadSuelo);
    Serial.print(" | Temp: "); Serial.print(reporte.temperatura); Serial.println(" C");
    
    Serial.println("ESTADO DE ACTUADORES:");
    Serial.print("  [BOMBA]: ");      Serial.println(reporte.bombaOn ? "ENCENDIDO (Riego)" : "APAGADO");
    Serial.print("  [FOCO]:  ");      Serial.println(reporte.focoOn  ? "ENCENDIDO (Calor)" : "APAGADO");
    Serial.print("  [VENTILADOR]: "); Serial.println(reporte.ventOn  ? "ENCENDIDO (Aire)"  : "APAGADO");
    Serial.println("----------------------------------\n");
  }
}