// библиотека для работы с датчиками MQ (Troyka-модуль)
#include <TroykaMQ.h>
#include <TroykaMeteoSensor.h>
 
// MARK: - Defines
#define PIN_MQ135     A0
#define PERIOD_MQ135  1000
#define PERIOD_METEO  5000

#define HEAD_MQ135    "MQ135"
#define HEAD_METEO    "METEO"

// MARK: - Global
String *output;
unsigned long memTime_MQ135;
unsigned long memTime_Meteo;
MQ135 mq135(PIN_MQ135);
TroykaMeteoSensor meteoSensor;

// - Outputs
void printOver();
void printHeader(String str);
void printBody(String str);
 
void setup() {
  initialCOM();
  initialMQ135();
  initialMeteo();
}
 
void loop() {
  updateDataSensors();
}

// - Initial
void initialCOM() {
  delay(1000);
  Serial.begin(9600);
  Serial.println("|Serial->Initialization is over!|");
  delay(1000);
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
