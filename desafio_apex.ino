#include <SPI.h>  
#include <SD.h>

#include <Servo.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SERVO_PORT 6 // Porta Digital 6
#define SD_PORT 4 // Porta Digital 4

#define SEALEVELPRESSURE_HPA (1013.25)
#define SERVO_INITIAL_ANGLE 180 // angulo inicial do Servo
#define SERVO_FINAL_ANGLE 90  // angulo final do Servo
#define THRESHOLD 0.2 // porcentagem de queda relativa ao apogeu para abertura do paraquedas

Adafruit_BME280 bme;
File dataFile;

float actualHeight;
float apogee = 0.0;
Servo s;
bool parachuteTriggered = false;

void setup() {
  SD.begin(SD_PORT);
  if(SD.exists("data.txt"))
    SD.remove("data.txt");
  
  logMessage("//inicio do log");
  s.attach(SERVO_PORT);
  s.write(SERVO_INITIAL_ANGLE);

  if (!bme.begin(0x76)) {
    logMessage("//nao foi encontrado um sensor BME280 valido, cheque as conexoes!");
    while (1);
  }

  actualHeight = bme.readAltitude(SEALEVELPRESSURE_HPA);
}


void loop() {
  actualHeight = bme.readAltitude(SEALEVELPRESSURE_HPA);
  if (actualHeight > apogee) {
    apogee = actualHeight;
  }
  else {
    if ((apogee - actualHeight > THRESHOLD * apogee) && !parachuteTriggered) {
      triggerParachute();
    }
  }
  logMessage(String(actualHeight,2));
  delay(100);
}

void triggerParachute() {
  String str = "//paraquedas acionado a: " + String(actualHeight,2);
  logMessage(str);
  s.write(SERVO_FINAL_ANGLE);
  parachuteTriggered = true;
  delay(1000);
  s.write(SERVO_INITAL_ANGLE);
}

void logMessage(String str){
  dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println(str);
  dataFile.close();  
}
