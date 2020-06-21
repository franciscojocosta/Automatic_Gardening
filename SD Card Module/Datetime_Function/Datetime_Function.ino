
#include <SPI.h>
#include <SD.h>
#include <Wire.h>     //needed because DS3231 uses I2C Bus
#include <RTClib.h>   //needed becuase we have ready-made functions of this librray
#include <dht.h>

#define deviceAddress 0x68
//i2c pins
  RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231
  dht DHT;

float startTime = 0;
float endTime = 0;


String date;
  struct date_time{
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second;
    };


void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());

  startTime = millis();
}

date_time get_dateTime() {

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
  DateTime nowTime = rtc.now();

  date_time today;
   //Serial.println(nowTime.year());
   
  today.year = nowTime.year();
  today.month = nowTime.month();
  today.day = nowTime.day();
  
  today.hour = nowTime.hour();
  today.minute = nowTime.minute();
  today.second = nowTime.second();
  
  return today;
 }


void setup() {
  Serial.begin(9600);
  date_time now_datetime;
  now_datetime = get_dateTime();

  //Serial.println(x);
}

void loop() {
  Serial.print (now_datetime.hour);

}
