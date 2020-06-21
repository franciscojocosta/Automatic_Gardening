#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "SdFat.h"



DateTime now;
RTC_DS3231 rtc;
File logfile;
const uint8_t SD_CS = 10; // SD chip select 

void dateTime(uint16_t* date, uint16_t* time) {
 DateTime now = rtc.now();
 //sprintf(timestamp, "%02d:%02d:%02d %2d/%2d/%2d \n", now.hour(),now.minute(),now.second(),now.month(),now.day(),now.year()-2000);
 Serial.println("yy");
// Serial.println(timestamp);
 // return date using FAT_DATE macro to format fields
 *date = FAT_DATE(now.year(), now.month(), now.day());

 // return time using FAT_TIME macro to format fields
 *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

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
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
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
  
  
  logfile.println("Unix Time (s),Date,Soil Temp (F),Air Temp (F),Soil Moisture Content (%),Relative Humidity (%),Heat Index (F),Sunlight Illumination (lux),Watering?");   //HEADER 
#if ECHO_TO_SERIAL
  Serial.println("Unix Time (s),Date,Soil Temp (F),Air Temp (F),Soil Moisture Content (%),Relative Humidity (%),Heat Index (F),Sunlight Illumination (lux),Watering?");
#endif ECHO_TO_SERIAL// attempt to write out the header to the file

  if (! logfile) {
    error("couldnt create file");
  }  
}

void loop()
{
  DateTime nowTime = rtc.now();
  logfile.print("1232"); // seconds since 2000
  logfile.print(",");
  logfile.print("2020");
  logfile.print("/");
  logfile.print("04");
  logfile.print("/");
  logfile.print("13");
  logfile.print(" ");
  logfile.print("20");
  logfile.print(":");
  logfile.print("12");
  logfile.print(":");
  logfile.print("58");
  logfile.print(nowTime.day());
  logfile.print("/");

  logfile.print(nowTime.month());
  logfile.print("/");

  logfile.print(nowTime.year());
  logfile.print(",");
 #if ECHO_TO_SERIAL
  Serial.print("123232"); // seconds since 2000
  Serial.print(",");
  Serial.print("2020", DEC);
  Serial.print("/");
  Serial.print("04", DEC);
  Serial.print("/");
  Serial.print("13", DEC);
  Serial.print(" ");
  Serial.print("20", DEC);
  Serial.print(":");
  Serial.print("12", DEC);
  Serial.print(":");
  Serial.print("58", DEC);
  Serial.print(nowTime.day());
  Serial.print("/");

  Serial.print(nowTime.month());
  Serial.print("/");

  Serial.print(nowTime.year());

#endif //ECHO_TO_SERIAL
  delay(50);
    //Write to SD card
  logfile.flush();
  delay(5000);

}
