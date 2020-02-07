#include <SPI.h>  
#include <SD.h>

#include <Servo.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SERVO_PORT 6 // Porta Digital 6
#define SD_PORT 4 // Porta Digital 4

#define SEALEVELPRESSURE_HPA (1013.25)
#define SERVO_INITIAL_ANGLE 0 // angulo inicial do Servo
#define SERVO_FINAL_ANGLE 10  // angulo final do Servo
#define THRESHOLD 0.3 // porcentagem de queda relativa ao apogeu para abertura do paraquedas

Adafruit_BME280 bme;
File dataFile;

float alturaInicial;
float alturaAtual;
float apogeu = 0.0;
Servo s;
bool paraquedasAcionado = false;

void setup() {
  SD.begin(SD_PORT);
  Serial.begin(9600);
  if(SD.exists("data.txt"))
    SD.remove("data.txt");
  
  logMessage("//inicio do log");
  s.attach(SERVO_PORT);

  if (!bme.begin(0x76)) {
    logMessage("//nao foi encontrado um sensor BME280 valido, cheque as conexoes!");
    while (1);
  }

  alturaInicial = bme.readAltitude(SEALEVELPRESSURE_HPA);
}


void loop() {
  alturaAtual = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
  if (alturaAtual > apogeu) {
    apogeu = alturaAtual;
  }
  else {
    if ((apogeu - alturaAtual > THRESHOLD * apogeu) && !paraquedasAcionado) {
      acionarParaquedas();
    }
  }
  logMessage(String(alturaAtual,2));
  delay(100);
}

void acionarParaquedas() {
  String str = "//paraquedas acionado a: " + String(alturaAtual,2);
  logMessage(str);
  for(int i = 0; i < SERVO_FINAL_ANGLE; i++)
    s.write(i);
  paraquedasAcionado = true;
}

void logMessage(String str){
  dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println(str);
  dataFile.close();  
}
