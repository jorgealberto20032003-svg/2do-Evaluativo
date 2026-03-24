#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <DHT.h>

// Sensores
const int sensorH = 2; 
#define DHTPIN 1 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Relés en S3
const int pinBomba = 6;      // IN1
const int pinFoco = 7;       // IN2
const int pinVentilador = 8;  // IN3

// Radio nRF24L01 (S3: CE=4, CSN=5)
RF24 radio(4, 5); 
const byte address[6] = "12321";

struct Payload {
  int humedadSuelo;
  float temperatura;
  bool bombaOn;
  bool focoOn;
  bool ventOn;
};

Payload datos;

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(pinBomba, OUTPUT);
  pinMode(pinFoco, OUTPUT);
  pinMode(pinVentilador, OUTPUT);
  
  // Apagar todo al inicio (HIGH = OFF)
  digitalWrite(pinBomba, HIGH);
  digitalWrite(pinFoco, HIGH);
  digitalWrite(pinVentilador, HIGH);

  SPI.begin(12, 13, 11, 5); 
  if (!radio.begin()) Serial.println("Error Radio");
  
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  datos.humedadSuelo = analogRead(sensorH);
  datos.temperatura = dht.readTemperature();

  // --- LÓGICA LOCAL ---
  // Bomba: Si suelo > 3200 (Seco)
  if(datos.humedadSuelo > 3200) { digitalWrite(pinBomba, LOW); datos.bombaOn = true; }
  else { digitalWrite(pinBomba, HIGH); datos.bombaOn = false; }

  // Foco: Si Temp < 19°C
  if(datos.temperatura < 19.0) { digitalWrite(pinFoco, LOW); datos.focoOn = true; }
  else { digitalWrite(pinFoco, HIGH); datos.focoOn = false; }

  // Ventilador: Si Temp > 28°C
  if(datos.temperatura > 29.0) { digitalWrite(pinVentilador, LOW); datos.ventOn = true; }
  else { digitalWrite(pinVentilador, HIGH); datos.ventOn = false; }

  // Enviar al WROOM
  radio.write(&datos, sizeof(Payload));
  
  delay(2000);
}