// библиотека для работы с датчиками MQ (Troyka-модуль)
#include <TroykaMQ.h>
#include <TroykaMeteoSensor.h>
#include <GPRS_Shield_Arduino.h>
 
// MARK: - Defines
#define PIN_MQ135     A0
#define PERIOD_MQ135  1000
#define PERIOD_METEO  5000

#define HEAD_SERIAL   "SERIAL"
#define HEAD_GPRS     "GPRS"
#define HEAD_MQ135    "MQ135"
#define HEAD_METEO    "METEO"

// MARK: - Global
String *output;
unsigned long memTime_MQ135;
unsigned long memTime_Meteo;
MQ135 mq135(PIN_MQ135);
GPRS gprs(Serial1);
TroykaMeteoSensor meteoSensor;

// - Outputs
void printOver();
void printHeader(String str);
void printBody(String str);
 
void setup() {
  initialCOM();
  initialGPRS();
  initialMQ135();
  initialMeteo();
}
 
void loop() {
  updateDataSensors();
}

// - Initial
void initialCOM() {
  Serial.begin(9600);
  while (!Serial) { }
  printHeader(HEAD_SERIAL);
  printBody("Serial init success!");
  printOver();
}

void initialGPRS() {
  Serial1.begin(9600);
  gprs.powerOn();
  // проверяем есть ли связь с GPRS устройством
  while (!gprs.init()) {
    printHeader(HEAD_GPRS);
    printBody("GPRS init error ...");
    printOver();
    delay(3000);
  }

  // выводим сообщение об удачной инициализации GPRS Shield
  printHeader(HEAD_GPRS);
  printBody("GPRS init success!");
  printOver();
}

void initialMQ135() {
  printHeader(HEAD_MQ135);
  printBody("Start calibrate sensor");
  printOver();
  delay(3000);
  mq135.calibrate();
  delay(5000);
  printHeader(HEAD_MQ135);
  printBody("Calibration is over!");
  printOver();

  printHeader(HEAD_MQ135);
  printBody("CleanValue Ro = " + String(mq135.getRo()));
  printOver();
}

void initialMeteo() {
  printHeader(HEAD_METEO);
  printBody("Start init sensor");
  meteoSensor.begin();
  printBody("Over init sensor");
  printOver();
}

// - Process
void updateDataSensors() {
  updateDataMQ135();
  updateDataMeteo();
}

void updateDataMQ135() {
  if (abs(millis() - memTime_MQ135) >= PERIOD_MQ135) {
    printHeader(HEAD_MQ135);
    printBody("Ratio = " + String(mq135.readRatio()) + "Rs/Ro" + "\t" + "CO2 = " + String(mq135.readCO2()) + "ppm");
    printOver();

    memTime_MQ135 = millis();
  }
}

void updateDataMeteo() {
  if (abs(millis() - memTime_Meteo) >= PERIOD_METEO) {
    switch (meteoSensor.read()) {
      case SHT_OK:
        printHeader(HEAD_METEO);
        printBody("T = " + String(meteoSensor.getTemperatureC()) + "C" + "\t" + "Humidity = " + String(meteoSensor.getHumidity()) + "%");
        printOver();
        break;
      // ошибка данных или сенсор не подключён
      case SHT_ERROR_DATA:
        printHeader(HEAD_METEO);
        printBody("Data error or sensor not connected");
        printOver();
        break; 
      // ошибка контрольной суммы
      case SHT_ERROR_CHECKSUM:
        printHeader(HEAD_METEO);
        printBody("Checksum error!");
        printOver();
        break;
    }

    memTime_Meteo = millis();
  }
}

// - Outputs
void printHeader(String sender) {
  Serial.print("|");
  Serial.print(sender);
  Serial.print("|");
  Serial.print("->");
  Serial.print("\t");
}

void printBody(String str) {
  Serial.print(str);
}

void printOver() {
  Serial.print("|");
  Serial.println();
}
