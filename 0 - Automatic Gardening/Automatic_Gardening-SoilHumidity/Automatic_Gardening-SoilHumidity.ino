
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

const int wateringbutton = 2;
const int LEDstatus = 3;
const int LEDstop = 5;
const int msensor = A0;
const int msAirValue = 600;   //Moister sensor Air value
const int msWaterValue = 240;  //Moister sensor Water value

bool watering = LOW;

int msvalues = 0;
int soilmoisturepercent=0;

float startTime = 0;
float endTime = 0;




void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  
  *date = FAT_DATE(now.year(), now.month(), now.day());       // return date using FAT_DATE macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());   // return time using FAT_TIME macro to format fields

  startTime = millis();
}


date_time get_dateTime() {

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
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
  Serial.begin(9600);
  
  pinMode(10, OUTPUT);

  pinMode(wateringbutton, INPUT);
  pinMode(LEDstatus, OUTPUT);
  pinMode(LEDstop, OUTPUT);
  pinMode(msensor, INPUT);
    
  lcd.begin(16,2); //define LCD:  16 columns and 2 rows
  lcd.backlight();

  Serial.print("Initializing SD card..."); 
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  SdFile::dateTimeCallback(dateTime);

  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      Serial.print("File created: "); 
      Serial.print(filename);
      myFile = SD.open(filename, FILE_WRITE);
      myFile.println("Date,Time,Temperature,Humidity,Soil Moister,Watering?");
      myFile.close();
      break;  // leave the loop!
    }
  }    
}

void loop()
{
  date_time now_datetime;
  now_datetime = get_dateTime();
  
  myFile = SD.open(filename, FILE_WRITE);
  Serial.println(filename);
  
  int temp = DHT.read11(DHT11_PIN);
  
  msvalues = analogRead(msensor);
  endTime = millis() - startTime;

  if (digitalRead(wateringbutton) == HIGH){
    
    Serial.print("Watering");
    watering = HIGH;
    
  } 
  if ( myFile && (endTime > 120000) ){
    
    startTime = millis();
    Serial.println("New data added to Sd file");
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
    myFile.println(msvalues);
    if (watering == HIGH){
        myFile.print(",");
        myFile.println("Watering");
    }
    else{
      myFile.print(",");
      myFile.println("Valve OFF");
    }
    
  }

///// IF STOP BUTTON IDLE \\\\\\\\\\\\\\\\\\\\\

  //if (digitalRead(stopbutton) == HIGH){
    
  //  Serial.print("STOP");
  //  digitalWrite(LEDstop, HIGH);
  //  stop();
  //}

///////////////////////////////////////////


 
  myFile.close();

// Print to LCD \\
  
//Sensors Data

  lcd.setCursor(0,0); //first row first column
  lcd.print("Humid:"); //16 characters poer line
   
  lcd.setCursor(7,0); 
  lcd.print(DHT.humidity); 

  lcd.setCursor(13,0); 
  lcd.print("%"); 

  lcd.setCursor(0,1); //first row second column
  lcd.print("Temp:"); //16 characters poer line
  
  lcd.setCursor(7,1); 
  lcd.print(DHT.temperature);
  
  lcd.setCursor(13,1); 
  lcd.print("C"); 

  delay(5000);

  lcd.clear();//Clean the screen

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
  
  delay (2500);
  lcd.clear();//Clean the screen

  }

    /*
Soil Moisture Reference
Air = 0%
Really dry soil = 10%
Probably as low as you'd want = 20%
Well watered = 50%
Cup of water = 100%
*/
