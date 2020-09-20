/******** Automtic Gardening *****************/
#include <Wire.h>                   //I2C Bus
#include <RTClib.h>                 //Real Time Clock
#include <SD.h>                     //SD Card
#include <LiquidCrystal_I2C.h>      // I2C Lcd Display
#include <dht.h>                    //DHT11 Humidity and Temperadure Sensor

#define deviceAddress 0x68  
#define DHT11_PIN 7                 // Defines DHT11 pins  

int Valve_relay = 8; 


void setup() {
  pinMode(Valve_relay, OUTPUT);
 
  Serial.begin(9600);
}

void loop() {
  digitalWrite(Valve_relay, HIGH);
  delay(1000);
  Serial.println(analogRead(A2));
  delay(2000);
    Serial.println(analogRead(A2));
  delay(2000);
  digitalWrite(Valve_relay, LOW);
    delay(1000);
  Serial.println(analogRead(A2));
  delay(2000);
    Serial.println(analogRead(A2));
  delay(2000);
  
}
