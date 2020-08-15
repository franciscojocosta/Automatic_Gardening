/******** Automtic Gardening *****************/

#include <SD.h>                   //SD Card
#include <Wire.h>                 //I2C Bus
#include <RTClib.h>               //Real Time Clock
#include <dht.h>                  //DHT11 Humidity and Temperadure Sensor
#include <LiquidCrystal_I2C.h>    // I2C Lcd Display

#define DHT11_PIN 7
#define deviceAddress 0x68

RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231
dht DHT;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //

File myFile;
File f;

struct date_time{
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second;
    };

String date;

char filename[] = "LOGGER00.txt";

bool  watering = LOW;

const int wateringbutton= 2;
const int LEDstatus = 3;
const int LEDstop = 5;
const int msensor = A0;
const int msAirValue = 600;   //Moister sensor Air value
const int msWaterValue = 240;  //Moister sensor Water value
const int water_high = 450; // Max soil moister threshold to water

int soilmst = 0;
int soilmst_percent=0;
int LCDstate = 0;
int water = 0;


float startTime = 0;
float endTime = 0;
float tLCD_update = 0;
float LCD_starttime = 0;




void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  
  *date = FAT_DATE(now.year(), now.month(), now.day());       // return date using FAT_DATE macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());   // return time using FAT_TIME macro to format fields

}

bool to_water( uint16_t soilmst, uint16_t water_count, uint16_t water_countino){
  water = 0;
  if (soilmst < water_high){
    water = water + 1;
  }
  if (water_count < 3){
    water = water + 1;
  }
  if (water_countino < 3){
    water = water + 1;
  }

  if (water = 3){
    return HIGH;
  }
  else{
    return LOW;
  }
  

}


date_time get_dateTime() {


  DateTime nowTime = rtc.now();

  date_time today;

  today.year = nowTime.year();
  today.month = nowTime.month();
  today.day = nowTime.day();
  
  today.hour = nowTime.hour();
  today.minute = nowTime.minute();
  today.second = nowTime.second();
  
  return today;
 }

void stop()
{
 while(1);
}


void setup()
{
  
  rtc.begin();
 //+ rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
  pinMode(10, OUTPUT);

  pinMode(wateringbutton, INPUT);
  pinMode(LEDstatus, OUTPUT);
  pinMode(LEDstop, OUTPUT);
  pinMode(msensor, INPUT);
    
  lcd.begin(16,2); //define LCD:  16 columns and 2 rows
  lcd.backlight();

 
  if (!SD.begin(4)) {
 
    return;
  }

  
  SdFile::dateTimeCallback(dateTime);

  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
     
    
      myFile = SD.open(filename, FILE_WRITE);
      myFile.println("Date,Time,Temperature,Humidity,Soil Moister, Soil Moister Percent,Watering?");
      myFile.close();
      break;  // leave the loop!
    }
  }    
}

void loop()
{
  delay(5000);
  date_time now_datetime;
  now_datetime = get_dateTime();
  
  myFile = SD.open(filename, FILE_WRITE);
  
  
  int temp = DHT.read11(DHT11_PIN);
  
  soilmst = analogRead(msensor);
  endTime = millis() - startTime;
  soilmst_percent = map (soilmst, 240, 620, 0, 100);
  
   if (digitalRead(wateringbutton) == HIGH){
    
    
    watering = HIGH;
    
  }
  
  if ( myFile && (endTime > 120000) ){
    
    startTime = millis();
   
    //date
    myFile.print(now_datetime.year, DEC);
    myFile.print("/");
    myFile.print(now_datetime.month, DEC);
    myFile.print("/");
    myFile.print(now_datetime.day, DEC);
    myFile.print(",");
    
    //time
    myFile.print(now_datetime.hour);
    myFile.print(':');
    myFile.print(now_datetime.minute);
    myFile.print(",");
    
    //sensors
    myFile.print(DHT.temperature);
    
    myFile.print(",");
    myFile.print(DHT.humidity);
    myFile.print(",");
    myFile.print(soilmst);
    myFile.print(",");
    myFile.print(soilmst_percent);

    //watering

    if (watering == HIGH){
       myFile.print(",");
      myFile.println("1");
      watering = LOW;
    }
    else{
       myFile.print(",");
      myFile.println("0");
    }
  }
  
  myFile.close();

// Print to LCD \\
  
    tLCD_update = millis() - LCD_starttime;
  
//Sensors Data
  if (tLCD_update >5000 && LCDstate == 0){
    lcd.clear();//Clean the screen
    LCDstate = 1;
    LCD_starttime = millis();

      // Humid \\
    
    lcd.setCursor(0,0); //first row first column
    lcd.print("Humid:"); //16 characters poer line
     
    lcd.setCursor(7,0); 
    lcd.print(DHT.humidity); 
  
    lcd.setCursor(13,0); 
    lcd.print("%"); 

    // Temp \\
    
    lcd.setCursor(0,1); //first row second column
    lcd.print("Temp:"); //16 characters poer line
    
    lcd.setCursor(7,1); 
    lcd.print(DHT.temperature);
    
    lcd.setCursor(13,1); 
    lcd.print("C"); 
    
  }
  tLCD_update = millis() - LCD_starttime;
  if (tLCD_update >5000 && LCDstate == 1){
    lcd.clear();//Clean the screen
    //lcd.clear();//Clean the screen
    
    LCDstate = 0;
    LCD_starttime = millis();
    
  // Date \\
    lcd.setCursor(0,0); 
    lcd.print(now_datetime.day); 
  
    lcd.setCursor(2,0); 
    lcd.print("/"); 
  
    lcd.setCursor(3,0); 
    lcd.print(now_datetime.month); 
  
    lcd.setCursor(5,0); 
    lcd.print("/"); 
    
    lcd.setCursor(6,0); 
    lcd.print(now_datetime.year); 
  
  // Time \\
  
        
    lcd.setCursor(12,1); 
    lcd.print(":"); 
       
    lcd.setCursor(13,1); 
    lcd.print(now_datetime.minute);
  
    lcd.setCursor(10,1); 
    lcd.print(now_datetime.hour);
    
  
  
    // Humid\\
  
    lcd.setCursor(0,1); 
    lcd.print("H"); 
       
    lcd.setCursor(1,1); 
    lcd.print(":");
  
    lcd.setCursor(2,1); 
    lcd.print(soilmst);
    
    lcd.setCursor(6,1); 
    lcd.print(soilmst_percent);
    
    lcd.setCursor(8,1); 
    lcd.print("%");
   
    
  
  }
}




    /*
Soil Moisture Reference
Air = 0%
Really dry soil = 10%
Probably as low as you'd want = 20%
Well watered = 50%
Cup of water = 100%
*/
