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
#define SERVO_FINAL_ANGLE 90  // angulo final do Servo
#define THRESHOLD 0.3 // porcentagem de queda relativa ao apogeu para abertura do paraquedas

Adafruit_BME280 bme;
File dataFile;

float alturaAtual;
float apogeu = 0.0;
Servo s;

void setup() {
  SD.begin(SD_PORT);
  dataFile = SD.open("data.txt", FILE_WRITE);

  s.attach(SERVO_PORT);
  s.write(SERVO_INITIAL_ANGLE);

  if (!bme.begin(0x76)) {
    dataFile.println("//nao foi encontrado um sensor BME280 valido, cheque as conexoes!");
    while (1);
  }

}


void loop() {
  alturaAtual = bme.readAltitude(SEALEVELPRESSURE_HPA);

  if (alturaAtual > apogeu) {
    apogeu = alturaAtual;
  }
  else {
    if (apogeu - alturaAtual > THRESHOLD * apogeu ) {
      acionarParaquedas();
    }
  }

  dataFile.println(alturaAtual);
  delay(100);
}

void acionarParaquedas() {
  String str = "//paraquedas acionado a: " + String(alturaAtual,2);
  dataFile.println(str);
  for(int ang=SERVO_INITIAL_ANGLE; ang<SERVO_FINAL_ANGLE; ang++){
    s.write(ang);
  }
}
