
//Purpose:
//  Sensor Node for Turkey Students
//
//Hardware - Prepared By Reha Yurdakul(rehay@ke.ibm.com)
//  MFNode
//  DHT11 - Temperature and Humidity Sensor
//  BMP180 - Barometric Pressure, Altitude and Temperature Sensor
//  Photoresistor
//  SIM900R
//
//Author
//  Kelvin Mwega (kelvinm@ke.ibm.com)
//
//Pin Assignment 
//    A2:  Light Sensor
//    A5:  SDA (BMP180)
//    A6:  SCL (BMP180)
//    D8:  DATA (DHT11)
//    D6:  GSM Enable/Toggle
//
//Version 1 - 21st April 2017
//  Test DHT11, BMP180 and photoresistor, logs values on console.



#include <mqtt.h>
#include<dht.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

dht DHT;
Adafruit_BMP085 bmp;

#define DHT11_PIN 8
int photoCellPin = 2;
int photoCellReading;

void setup() {
  Serial.begin(9600);
  digitalWrite(A2, INPUT_PULLUP);
  delay(1000);
  checkBMP();
  checkDHT();
  delay(1000);
  blinkLED(13,2,500);
  Serial.println("--------------------------------- ");
  Serial.println("--------------------------------- ");
}

void loop() {
  blinkLED(13,5,100);
  printTestValues();
  delay(3000);
}

//##########  Test Functions  ##########

void printTestValues(){
  Serial.print(" Humidity - " );
  Serial.println(measureLevelHumidity());
  Serial.print(" Temparature - ");
  Serial.println(measureLevelTemperature());
  Serial.print(" Light - ");
  Serial.println(measureLevelLight());
  Serial.print(" Altitude - ");
  Serial.println(measureAltitude());
  Serial.print(" Real Altitude - ");
  Serial.println(measureRealAlt());
  Serial.print(" Pressure - ");
  Serial.println(measureLevelPressure());  
  Serial.print(" BMP180 Temp - ");
  Serial.println(measureBMPTemp());
  Serial.println("--------------------------------- ");
}

void checkDHT(){
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.println("DHT11 Status :: OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("DHT11 Status :: Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("DHT11 Status :: Time out error,\t"); 
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.println("DHT11 Status :: Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.println("DHT11 Status :: Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.println("DHT11 Status :: Ack High error,\t");
        break;
    default: 
    Serial.println("Unknown error,\t"); 
    break;
  }
}

void checkBMP(){
  if (!bmp.begin()){
    Serial.println("BMP180 Unavailable. Check Connections");
    while(1){}
  } else {
    Serial.println("BMP180 Status :: OK,\t"); 
  }
}

//########## Sensors Read Functions ##########

//########## DHT11 Read ##########

float measureLevelTemperature() {     
  float temp = DHT.temperature; //Degrees Celcius
  return temp;
}

float measureLevelHumidity() { 
  float hum = DHT.humidity; //Percentage
  return hum;
}

//########## BMP180 Read ##########

float measureLevelPressure() {   
  float pressure = bmp.readPressure(); //Pascals       
  return pressure; 
}

float measureAltitude(){
  float alt = bmp.readAltitude(); //Meters
  return alt;
}

float measureRealAlt(){
  float realAlt = bmp.readAltitude(101200); //Real altitude adjusted to Sea level air pressure in pascals  (101200 - Mombasa)
  return realAlt;
}

float measureBMPTemp(){
  float bmpTemp = bmp.readTemperature(); //Degrees Celcius
  return bmpTemp;
}

//########## Photoresistor Read ##########

int measureLevelLight() {           
  return 1024-analogRead(A2); //Analogue value : 0 - Darkest : 1024 - Brightest
}

//########## Cogs ##########

void blinkLED(int ledID, int repeat, int wait) {
  if (repeat == 999) { 
    digitalWrite(ledID, HIGH); 
    return; 
  }
  if (repeat == 0) { 
    digitalWrite(ledID, LOW); 
    return; 
  }
  for (int i = 0; i < repeat; i++) { 
    digitalWrite(ledID, HIGH); 
    delay(wait);
    digitalWrite(ledID, LOW); 
    delay(wait);
  }
  delay(1000);
}
