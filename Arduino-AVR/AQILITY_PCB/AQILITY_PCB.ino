// 
// AQILITY (Air Quality Index Aquisition Ability)
// Created by Pan Ziyue and Chow Zijie 2014
// 

#include "dht11.h"

/* Start RainSensor/TempSensor */
const int numReadings = 10;
const int frequency = 1;    // How many probes per second?
const int caliOff = -10;

dht11 DHT11;
/* End RainSensor/TempSensor */

/* Start PinNames */
const int mq131Pin = A0;
const int mq135Pin = A1;
const int mq9Pin = A2; // Flammable gases
const int dhtPin = A3;
const int mq92Pin = A4; // Carbon monoxide
const int mq136Pin = A5; // Hydrogen Sulfide (H2S)
const int p1Pin = 8;
const int p2Pin = 9;
/* End PinNames */

/* Start SensorValues */
float mq131Voltage;
float mq135Voltage;
float mq9Voltage;
float mq92Voltage;
float mq136Voltage;

int humidityReadings[10];
int humidityIndex = 0;
int humidityTotal = 0;
int humidityAverage = 0;
float temperature = 0.0;
float humidity = 0.0;
/* End SensorValues*/

/* Start DustDuino Variable Import */
unsigned long starttime;

unsigned long triggerOnP1;
unsigned long triggerOffP1;
unsigned long pulseLengthP1;
unsigned long durationP1;
boolean valP1 = HIGH;
boolean triggerP1 = false;

unsigned long triggerOnP2;
unsigned long triggerOffP2;
unsigned long pulseLengthP2;
unsigned long durationP2;
boolean valP2 = HIGH;
boolean triggerP2 = false;

float ratioP1 = 0;
float ratioP2 = 0;
unsigned long sampletime_ms = 30000;
float countP1;
float countP2;

float concLarge=0.0;
float concSmall=0.0;

// Averaging variables starts here
float pm25Readings[5];
int pm25Index = 0;
float pm25Total = 0.0;
float pm25Average = 0.0;
float pm10Readings[5];
int pm10Index = 0;
float pm10Total = 0.0;
float pm10Average = 0.0;
/* End DustDuino Variable Import */

/* Start Serial Essentials */
int incomingByte = 0;
/* End Serial Essentials */

void setup() {
  // Init serial
  Serial.begin(9600);
  
  // Set pinMode just in case
  pinMode(dhtPin,INPUT);
  pinMode(p1Pin,INPUT);
  pinMode(p2Pin,INPUT);
  
  // Initial read of temperature and humidity from the sensor.
  // May take up to 20 seconds before the Arduino responds
  for (int i=0; i<10; i++) {
    temperatureAndHumidityUpdate();
  }
}

void loop() {
  // Retreive all data from sensors here (except from DHT11)
  getDustData();
  mq131Voltage = (analogRead(mq131Pin)/1024.0)*5.0;
  mq135Voltage = (analogRead(mq135Pin)/1024.0)*5.0;
  mq9Voltage = (analogRead(mq9Pin)/1024.0)*5.0;
  mq92Voltage = (analogRead(mq92Pin)/1024.0)*5.0;
  mq136Voltage = (analogRead(mq136Pin)/1024.0)*5.0;
  
  // Check for RPi asking for serial data transmission
  incomingByte = Serial.read();
  // Begin serial data transmission, 9600 baud
  if (incomingByte == 46) {
    temperatureAndHumidityUpdate(); // This takes very long (read from DHT11)
    Serial.print(mq131Voltage);
    Serial.print(",");
    Serial.print(mq135Voltage);
    Serial.print(",");
    Serial.print(mq9Voltage);
    Serial.print(",");
    Serial.print(mq92Voltage);
    Serial.print(",");
    Serial.print(mq136Voltage);
    Serial.print(",");
    Serial.print(pm25Average);
    Serial.print(",");
    Serial.print(pm10Average);
    Serial.print(",");
    // Calculate AQI rating
    //Serial.print("AQI: ");
    if (pm25Average>250.0 || pm10Average>420.0) {
      Serial.print("HAZARDOUS");
    } else if ((pm25Average<=250.0 && pm25Average>150.0) || (pm10Average<=420.0 && pm10Average>350.0)) {
      Serial.print("Very Unhealthy");
    } else if ((pm25Average<=150.0 && pm25Average>55.0) || (pm10Average<=350.0 && pm10Average>150.0)) {
      Serial.print("Unhealthy");
    } else if ((pm25Average<=55.0 && pm25Average>12.0) || (pm10Average<=150.0 && pm10Average>50.0)) {
      Serial.print("Moderate");
    } else if (pm25Average<=12.0 && pm10Average<=50.0) {
      Serial.print("Good");
    }
    Serial.print(",");
    Serial.print(temperature);
    Serial.print(",");
    Serial.println(humidity);
  }  
}

void temperatureAndHumidityUpdate() {
  switch(DHT11.read(dhtPin)) {
    case DHTLIB_OK: break;
    case DHTLIB_ERROR_CHECKSUM:
      //Serial.println("Error Checksum!!");
      goto fail;
      break;
    case DHTLIB_ERROR_TIMEOUT:
      //Serial.println("Error Timeout!!");
      goto fail;
      break;
    default:
      //Serial.println("Unknown Error!!");
      goto fail;
      break;
  }
  
  humidityTotal= humidityTotal - humidityReadings[humidityIndex];
  humidityReadings[humidityIndex] = DHT11.humidity;
  humidityTotal= humidityTotal + humidityReadings[humidityIndex];
  humidityIndex = humidityIndex + 1;
  if (humidityIndex >= numReadings)
    humidityIndex = 0;
  humidityAverage = humidityTotal / numReadings;
  
  temperature = DHT11.temperature;
  humidity = humidityAverage;
  
  fail: // Failed probe, skip all code and repeat
  delay((int)1000/frequency);
}

void getDustData() {
  valP1 = digitalRead(p1Pin); // Digital Pin 8
  valP2 = digitalRead(p2Pin); // Digital Pin 9
  
  if(valP1 == LOW && triggerP1 == false){
    triggerP1 = true;
    triggerOnP1 = micros();
  }
  
  if (valP1 == HIGH && triggerP1 == true){
    triggerOffP1 = micros();
    pulseLengthP1 = triggerOffP1 - triggerOnP1;
    durationP1 = durationP1 + pulseLengthP1;
    triggerP1 = false;
  }
  
  if(valP2 == LOW && triggerP2 == false){
    triggerP2 = true;
    triggerOnP2 = micros();
  }
  
  if (valP2 == HIGH && triggerP2 == true){
    triggerOffP2 = micros();
    pulseLengthP2 = triggerOffP2 - triggerOnP2;
    durationP2 = durationP2 + pulseLengthP2;
    triggerP2 = false;
  }
    
  if ((millis() - starttime) > sampletime_ms) {
    ratioP1 = durationP1/(sampletime_ms*10.0);  // Integer percentage 0=>100
    ratioP2 = durationP2/(sampletime_ms*10.0);
    countP1 = 1.1*pow(ratioP1,3)-3.8*pow(ratioP1,2)+520*ratioP1+0.62;
    countP2 = 1.1*pow(ratioP2,3)-3.8*pow(ratioP2,2)+520*ratioP2+0.62;
    float PM10count = countP2;
    float PM25count = countP1 - countP2;
    
    // Avoid wasting computational cycles by not processing bad data
    if (PM10count > 0.62) {
      // first, PM10 count to mass concentration conversion
      double r10 = 2.6*pow(10,-6);
      double pi = 3.14159;
      double vol10 = (4/3)*pi*pow(r10,3);
      double density = 1.65*pow(10,12);
      double mass10 = density*vol10;
      double K = 3531.5;
      
      // next, PM2.5 count to mass concentration conversion
      double r25 = 0.44*pow(10,-6);
      double vol25 = (4/3)*pi*pow(r25,3);
      double mass25 = density*vol25;
      
      concLarge = (PM10count)*K*mass10;
      concSmall = (PM25count)*K*mass25;
      
      // Calculating averages here
      pm25Total = pm25Total - pm25Readings[pm25Index];
      pm25Readings[pm25Index] = concSmall;
      pm25Total = pm25Total + pm25Readings[pm25Index];
      pm25Index++;
      if (pm25Index >= 5)
        pm25Index=0;
      pm25Average = pm25Total/5;
      
      pm10Total = pm10Total - pm10Readings[pm10Index];
      pm10Readings[pm10Index] = concLarge;
      pm10Total = pm10Total + pm10Readings[pm10Index];
      pm10Index++;
      if (pm10Index >= 5)
        pm10Index=0;
      pm10Average = pm10Total/5;
    }
  
    durationP1 = 0;
    durationP2 = 0;
    starttime = millis();
  }
}
