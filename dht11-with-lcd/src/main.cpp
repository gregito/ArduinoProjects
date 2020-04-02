#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <SD.h>

#define DHTPIN 7

#define DHTTYPE DHT11
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
uint32_t oneMin = 60000;

const int chipSelect = 5;
const String logFileName = "datalog.txt";

void configureTempHumSensor();
void configureSDCard();
void printResultToSerial(String dataType, float data);
void printHumidityToLCD(float humidity, float line);
void printTemperatureToLCD(float temp, float line);
void writeDataToSDCard(String data);


void setup(){
  Serial.begin(9600);
  Serial.println("------");
  Serial.println("Configuration has started");
  Serial.println("------");
  configureSDCard();
  configureTempHumSensor();
  delayMS = oneMin;
  Serial.println("------");
  Serial.println("Configuration has finished");
  Serial.println("------");
}

void loop()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  String dataString = "";

  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    lcd.setCursor(0,0); 
    lcd.print("Temp: ERROR");
    dataString += "null";
  } else {
    printResultToSerial("Temperature", event.temperature);
    printTemperatureToLCD(event.temperature, 0);
    dataString += event.temperature;
  }

  dataString += ",";

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    lcd.setCursor(0,1); 
    lcd.print("Humidity: ERROR");
    dataString += "null";
  } else {
    printResultToSerial("Humidity", event.relative_humidity);
    printHumidityToLCD(event.relative_humidity, 1);
    dataString += event.relative_humidity;
  }

  writeDataToSDCard(dataString);

  delay(delayMS);
  Serial.println();

}

// ------------------------------------------ //

void printResultToSerial(String dataType, float data) {
  Serial.print(dataType);
  Serial.print(" recieved (");
  Serial.print(data);
  Serial.println("). About to send it to the LCD display.");
}

void printHumidityToLCD(float humidity, float line) {
  lcd.setCursor(0,line);
  lcd.print("Humidity: ");
  lcd.print((int) humidity);
  lcd.print("%");
}

void printTemperatureToLCD(float temp, float line) {
  lcd.setCursor(0,line);
  lcd.print("Temp: ");
  lcd.print((int) temp);
  lcd.print((char)223);
  lcd.print("C");
}

void configureTempHumSensor() {
  dht.begin();
  lcd.begin(16, 2);
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  //delayMS = sensor.min_delay / 1000;
}

void configureSDCard() {
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("Card initialized.");
}

void writeDataToSDCard(String data) {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
   if (dataFile)
   {
    dataFile.println(data);
    dataFile.close();
    Serial.println(data);
   }
   else 
   {
    Serial.println("error opening " + logFileName);
   }
}