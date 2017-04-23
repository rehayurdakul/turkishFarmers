
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
//
//Version 2 - 23rd April 2017
//  Sends sensor values via MQTT to realtime.ngi.ibm.com mqtt server.



#include <mqtt.h>
#include<dht.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

dht DHT;
Adafruit_BMP085 bmp;


#define DHT11_PIN 8
#define LED13     13 
#define GSM_IGN   6                // GSM Module pin 6
#define DEVICE_IDNo "WN001"     // MFNode Device ID    

char server[] = "realtime.ngi.ibm.com";
char clientId[34] = "WN001";
char topic[26] = "IOC/Ke/Data";


byte    mqttMessage[127];
int     mqttMessageLength = 0;
int photoCellPin = 2;
int photoCellReading;

int defaultLightOn = 500;

void setup() {
  
  digitalWrite(A2, INPUT_PULLUP);
  delay(1000);
  checkBMP();
  checkDHT();
  setUpBoard();
  delay(1000);
  blinkLED(13,2,500);
  
}

void loop() {
  blinkLED(13,5,100);
  checkDHT();
  //printTestValues();
  payloadConstructor();
  delay(3000);
}


void setUpBoard(){
  //sensors.begin();
  blinkLED(13,10,100);
  initHWPins();
  blinkLED(13,1,defaultLightOn);
  gsm_is_ready_MODULE();
  blinkLED(13,2,defaultLightOn);
  blinkLED(13,3,defaultLightOn);
  gsm_is_ready_TCP();
  blinkLED(13,4,defaultLightOn);
  sendHelloMsg();
}

void sendHelloMsg()
{
  char aBuff[18], dataToSend[60], fBuff[9];
  strcpy(dataToSend, "Hallo from Farmers Node ID 001"); 
   sendMQTTMessage(clientId, server, topic, dataToSend);
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

int measureLevelTemperature() {     
  int temp = DHT.temperature; //Degrees Celcius
  return temp;
}

int measureLevelHumidity() { 
  int hum = DHT.humidity; //Percentage
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

void initHWPins() {
  pinMode(13, OUTPUT);
  pinMode(GSM_IGN, OUTPUT);
  digitalWrite(GSM_IGN,HIGH);    
}  

void payloadConstructor(){
  
  char strToSend[100];
  char buffer[7];
  
  blinkLED(13,3,100);

  strcpy(strToSend, DEVICE_IDNo);
  strcat(strToSend, ",");
  itoa(measureLevelTemperature(),buffer,10); //(integer, yourBuffer, base)
  strcat(strToSend, buffer);
  strcat(strToSend, ",");
  itoa(measureLevelHumidity(),buffer,10); 
  strcat(strToSend, buffer);
  strcat(strToSend, ",");
  dtostrf(measureLevelPressure(), 6, 0, buffer);
  strcat(strToSend, buffer);
  strcat(strToSend, ",");
  dtostrf(measureAltitude(), 5, 2, buffer);
  strcat(strToSend, buffer);
  strcat(strToSend, ",");
  dtostrf(measureRealAlt(), 5, 2, buffer);
  strcat(strToSend, buffer);
  strcat(strToSend, ",");
  dtostrf(measureBMPTemp(), 5, 2, buffer);
  strcat(strToSend, buffer);
  strcat(strToSend, ",");
  itoa(measureLevelLight(), buffer, 10);
  strcat(strToSend, buffer);

  sendMQTTMessage(clientId, server, topic, strToSend);
  
}

void sendMQTTMessage(char* clientId, char* brokerUrl,  char* topic, char* message){  
  blinkLED(13,999,defaultLightOn);
  mqttMessageLength = 16 + strlen(clientId);
  mqtt_connect_message(mqttMessage, clientId);          //prepare MQTTConnect Message -> mqttMessage
  gsm_send_tcp_MQTT_byte(brokerUrl,mqttMessage,mqttMessageLength);
  mqttMessageLength = 4 + strlen(topic) + strlen(message);
  mqtt_publish_message(mqttMessage, topic, message);   // prepare MQTTPublish Message -> mqTTMessage
  gsm_send_tcp_MQTT_byte(brokerUrl,mqttMessage, mqttMessageLength);
  blinkLED(13,0,100);
}


///    *************************************************************************
///    *************************************************************************
///    ****             GSM/TCP FUNCTIONS   GSM/TCP FUNCTIONS               ****
///    ****             GSM/TCP FUNCTIONS   GSM/TCP FUNCTIONS               ****
///    ****             GSM/TCP FUNCTIONS   GSM/TCP FUNCTIONS               ****
///    *************************************************************************
///    *************************************************************************

boolean gsm_send_tcp_MQTT_byte( char* mqttServer, byte* message, int lenMessage) {
x:
  if (gsm_is_ready_TCPSERVER(mqttServer,"1883") == false) goto x;            
  gsm_send("AT+CIPSEND"); 
  delay(300); 
  if ( gsm_response_check(">") == false) return false;
  for (int j = 0; j < lenMessage; j++)  Serial.write(message[j]); 
  Serial.write(byte(26)); 
  delay(2000); 
  gsm_response_check("SEND OK");     //dont use response.. because Data Transmit Check is counts!
  gsm_send("AT+CIPACK");  
  delay(500);   

  if ( gsm_response_check("Data Transmit Check") == false) { 
    goto x;
  }
  return true;
}


boolean gsm_send_tcp( char* message) {
x:
  if (gsm_is_ready_TCPSERVER("198.100.31.2","19940") == false) goto x;        //VPS
  gsm_send("AT+CIPSEND"); 
  delay(300); 
  if ( gsm_response_check(">") == false) return false;
  gsm_send(message);
  Serial.write(byte(26)); 
  delay(2000); 
  gsm_response_check("SEND OK");     //dont use response.. because Data Transmit Check is counts!
  gsm_send("AT+CIPACK");  
  delay(500);   
  if ( gsm_response_check("Data Transmit Check") == false) { 
    goto x;
  }
  return true;
}


boolean gsm_is_ready_TCPSERVER(char* server_ip, char* server_port) { 
  char at_command[50];
  strcpy(at_command, "AT+CIPSTART=\"TCP\",\"" );
  strcat(at_command, server_ip);
  strcat(at_command, "\",\"");
  strcat(at_command, server_port);
  strcat(at_command, "\"");
x:
  gsm_send("AT+CIPSTATUS"); 
  delay(300); 
  if ( gsm_response_check("STATE: CONNECT OK") == true) return true;
  gsm_is_ready_TCP();
  gsm_send(at_command); 
  delay(300); 
  gsm_response_check("OK");    // not important... does not helps !!
  goto x;
}


boolean gsm_is_ready_TCP() { 
x:
  digitalWrite(LED13, LOW);
  gsm_send("AT+CIPSTATUS"); 
  delay(300); 
  if ( gsm_response_check("STATE: IP STATUS") == true) {   
    digitalWrite(LED13, HIGH);
    return true; 
  }
  gsm_is_ready_GPRS();
  gsm_send("AT+CIPSHUT"); 
  delay(300); 
  if ( gsm_response_check("SHUT OK") == false ) goto x;
  gsm_send("AT+CSTT=\"internet\""); 
  delay(1000); 
  if ( gsm_response_check("OK") == false ) goto x;
  gsm_send("AT+CIICR"); 
  delay(1000); 
  if ( gsm_response_check("OK") == false ) goto x;
  gsm_send("AT+CIFSR"); 
  delay(300); 
  if ( gsm_response_check(".") == false ) goto x;
  goto x;
}

boolean gsm_is_ready_GPRS() {
  int reps = 0;
x:
  gsm_send("AT+CGATT=1"); 
  delay(300); 
  gsm_response_check("OK");
  gsm_send("AT+CGATT?"); 
  delay(300); 
  if ( gsm_response_check("+CGATT: 1") == true) {
    return true; 
  }
  if (reps == 7)
  {
    gsm_is_ready_NETWORK();
    reps = 0;
    goto x;
  }
  reps++;
  goto x;
}


boolean gsm_is_ready_NETWORK() {
  int noOfTry=0;
x:
  noOfTry++;
  gsm_send("AT+CREG?"); 
  delay(1000); 
  if ( gsm_response_check("+CREG: 0,1") == true) return true; 
  if (noOfTry == 3) { 
    noOfTry = 0; 
    gsm_is_ready_MODULE();
  }
  goto x;
}

boolean gsm_is_ready_MODULE() {
  int noOfTry=0;
  Serial.begin(19200); 
x:
  noOfTry++;
  gsm_send("AT"); 
  delay(500); 
  if ( gsm_response_check("OK") == true) return true;
  if (noOfTry == 3) {
    noOfTry = 0;
    gsm_toggleIGN();
  }

  goto x;
}


boolean gsm_response_check(String expected_response) {
  String  gprs_resp_str;

  Serial.flush();
  gprs_resp_str = gsm_read();

  if ( expected_response == "Data Transmit Check") {     //Speciall check the response !!
    int indexOfComma;
    char checkChar='0';
    indexOfComma= gprs_resp_str.lastIndexOf(',');    // , sign before number char left for Tx.. Should be 0 !
    if ( gprs_resp_str.charAt(indexOfComma+1) == checkChar) {
      return true;
    }
    else return false;  //still there are chars to send... NOT POSSIBLE ! reTry to SEND !
    return true;
  }

  if ( expected_response == "GET_DATE_TIME") {     //not response check.. just GET DATE/TIME
    int indexOfComma;
    char checkChar='0';
    indexOfComma= gprs_resp_str.lastIndexOf(',');    // , sign before number char left for Tx.. Should be 0 !
    if ( gprs_resp_str.charAt(indexOfComma+1) == checkChar) {
      return true;
    }
    else return false;  //still there are chars to send... NOT POSSIBLE ! reTry to SEND !
    return true;
  }
  if (gprs_resp_str.indexOf(expected_response) > -1){
    return true;
  } 
  else {
    return false;
  }
  return false;   //should never come here !
}

String gsm_read(){
  String gsmString;
  while (Serial.available()) gsmString = gsmString + (char)Serial.read();
  return gsmString;
}
  

void gsm_send(char* command) {
  Serial.println(command);
  delay(2500);
}

void gsm_toggleIGN() {
  digitalWrite(GSM_IGN, HIGH);   
  delay(1000);
  digitalWrite(GSM_IGN, LOW);
  delay(2000);
  digitalWrite(GSM_IGN, HIGH);   
  delay(3000);

}

void gsm_shutdown() {
  Serial.begin(19200);
  Serial.flush();
  Serial.println("AT+CPOWD=0");
  delay(2000);
  Serial.flush();
  Serial.println("AT");
  delay(500);
  Serial.end();
}




