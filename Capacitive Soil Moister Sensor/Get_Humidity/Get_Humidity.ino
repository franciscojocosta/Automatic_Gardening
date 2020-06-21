#include <Wire.h> 
 
int msensor = A0; // moisture sensor is connected with the analog pin A1 of the Arduino
int msvalue = 0; // moisture sensor value 
 
 
void setup()
{Serial.begin(9600); 
pinMode(msensor, INPUT);
}
 
void loop()
{
    msvalue = analogRead(msensor);
  Serial.println(msvalue);
  delay(1000); 
}

/*
Soil Moisture Reference
Air = 0%
Really dry soil = 10%
Probably as low as you'd want = 20%
Well watered = 50%
Cup of water = 100%
*/
