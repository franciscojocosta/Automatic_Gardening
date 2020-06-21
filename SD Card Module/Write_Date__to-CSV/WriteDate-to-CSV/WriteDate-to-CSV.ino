#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "SdFat.h"



DateTime now;
RTC_DS3231 rtc; //the object rtc is created from the class RTC_DS3231 
File logfile;
const uint8_t SD_CS = 10; // SD chip select 
#define deviceAddress 0x68 // Defines I2C RTC adress.





void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while(1);
}





 void stop()
{
 while(1);
}




void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");


  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
 
   // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      
      logfile = SD.open(filename, FILE_WRITE | O_APPEND); 
      break;  // leave the loop!
    }
  }
    Serial.print("Logging to: ");
  Serial.println(filename);

  if (! logfile) {
    error("couldnt create file");
  }  
}






void loop()
{

  
  DateTime nowTime = rtc.now();
  logfile.print("1232"); // test
 
 //--read Date and Tme from DS3231 using the method now()---------------------;
  //--store the Date and Time into another user define object bamed nowTime
 

  //---show Day and Date on Top Line of LCD--------------
  int x = nowTime.dayOfTheWeek(); //dyOfTheWeek() is a pre-defined method

  Serial.print(nowTime.day());
  Serial.print("/");

  Serial.print(nowTime.month());
  Serial.print("/");

  Serial.print(nowTime.year());

  Wire.beginTransmission(deviceAddress); //START, Roll Cal
  Wire.write(0x00); //set SEC Register address
  Wire.endTransmission(); //Execute the above queued data, ACK, STOP

  Wire.requestFrom(deviceAddress, 3);   //SEC, MIN, and HRS to read from RTC as BCD
  byte bcdSeconds = Wire.read();
  byte bcdMinutes = Wire.read();
  byte bcdHours = Wire.read();
  //-----------------------------
  
  Serial.print("  Time: ");
  //show HRS--
  Serial.print(bcdHours >> 4);
  Serial.print(bcdHours & 0x0F);
  Serial.print(':');

  //show MIN--
  Serial.print(bcdMinutes >> 4);
  Serial.print(bcdMinutes & 0x0F);
  Serial.print(':');
  
  //shiw SEC
  Serial.print(bcdSeconds >> 4);
  Serial.print(bcdSeconds & 0x0F);
  Serial.println();
  
  delay(50);
  logfile.flush();
  
  stop();

}
