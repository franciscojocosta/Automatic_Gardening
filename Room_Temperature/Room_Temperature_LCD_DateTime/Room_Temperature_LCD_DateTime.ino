/** This example shows how to read and write data to and from an SD card file   
 **The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
   
 */
 

#include <SPI.h>
#include <SD.h>
#include <Wire.h>     //needed because DS3231 uses I2C Bus
#include <RTClib.h>   //needed becuase we have ready-made functions of this librray
#include <dht.h>

#include <LiquidCrystal_I2C.h>
#define x=4
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //


#define DHT11_PIN 7
#define deviceAddress 0x68

const int stopbutton = 2;
const int LEDstatus = 3;
const int LEDstop = 5;
RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231
dht DHT;

char filename[] = "LOGGER00.txt";

float startTime = 0;
float endTime = 0;

File myFile;
File f;

//i2c pins


String date;

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());

  startTime = millis();
}


 void stop()
{
 while(1);
}


void setup()
{
      lcd.begin(16,2);
    lcd.backlight();

 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
  Serial.print("Initializing SD card...");
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //WE define our LCD 16 columns and 2 rows

  //lcd.backlight(); Power off the back light
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  
  SdFile::dateTimeCallback(dateTime);
   pinMode(stopbutton, INPUT);
   pinMode(LEDstatus, OUTPUT);
   pinMode(LEDstop, OUTPUT);
  
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      Serial.print("File created: "); 
      Serial.print(filename);
      myFile = SD.open(filename, FILE_WRITE);
      myFile.println("Date,Time,Temperature,Humidity");
      myFile.close();
      // Ler se ficheiro está em branco. se sim adicionar cabeçalho myFile.println("Date,Time,Temperatura,Humidity");
      
      break;  // leave the loop!
    }
  }  


 // myFile = SD.open("test5.txt", FILE_WRITE);
  
  
  
}

void loop()
{
  DateTime nowTime = rtc.now();
  int temp = DHT.read11(DHT11_PIN);
  myFile = SD.open(filename, FILE_WRITE);
  Serial.println(filename);
  // if the file opened okay, write to it:

  endTime = millis() - startTime;
  if ( myFile && (endTime > 120000) ){
    startTime = millis();
    Serial.println("DSSDSD");
    myFile.print(nowTime.year(), DEC);
    myFile.print("/");
    myFile.print(nowTime.month(), DEC);
    myFile.print("/");
    myFile.print(nowTime.day(), DEC);


    myFile.print(","); 

    
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
    myFile.print(bcdHours >> 4);
    myFile.print(bcdHours & 0x0F);
    myFile.print(':');
  
    //show MIN--
    myFile.print(bcdMinutes >> 4);
    myFile.print(bcdMinutes & 0x0F);
    myFile.print(",");

    myFile.print(DHT.temperature);
    myFile.print(",");
    myFile.println(DHT.humidity);
  
    Serial.println("done.");
  }
    // close the file:
   // myFile.close();
  Serial.println(digitalRead(stopbutton));
  if (digitalRead(stopbutton) == HIGH){
    Serial.print("ON");
    digitalWrite(LEDstop, HIGH);
    stop();
  }
 
  myFile.close();

  lcd.setCursor(0,0); //we start writing from the first row first column
  lcd.print("Humid:"); //16 characters poer line
   
  lcd.setCursor(7,0); //we start writing from the first row first column
  lcd.print(DHT.humidity); //16 characters poer line

  lcd.setCursor(13,0); //we start writing from the first row first column
  lcd.print("%"); //16 characters poer line

  lcd.setCursor(0,1); //we start writing from the first row first column
  lcd.print("Temp:"); //16 characters poer line
  
  lcd.setCursor(7,1); //we start writing from the first row first column
  lcd.print(DHT.temperature); //16 characters poer line
  
  lcd.setCursor(13,1); //we start writing from the first row first column
  lcd.print("C"); //16 characters poer line

  delay(10000);


   lcd.clear();//Clean the screen
  lcd.setCursor(6,0); //we start writing from the first row first column
  lcd.print(nowTime.year()); //16 characters poer line
   
  lcd.setCursor(5,0); //we start writing from the first row first column
  lcd.print("/"); //16 characters poer line

  lcd.setCursor(3,0); //we start writing from the first row first column
  lcd.print(nowTime.month()); //16 characters poer line
   
  lcd.setCursor(2,0); //we start writing from the first row first column
  lcd.print("/"); //16 characters poer line

  lcd.setCursor(0,0); //we start writing from the first row first column
  lcd.print(nowTime.day()); //16 characters poer line
   


  
      Wire.beginTransmission(deviceAddress); //START, Roll Cal
    Wire.write(0x00); //set SEC Register address
    Wire.endTransmission(); //Execute the above queued data, ACK, STOP
  
    Wire.requestFrom(deviceAddress, 3);   //SEC, MIN, and HRS to read from RTC as BCD
    byte bcdSeconds = Wire.read();
    byte bcdMinutes = Wire.read();
    byte bcdHours = Wire.read();
    //--------------------------

   lcd.setCursor(11,1); //we start writing from the first row first column
   lcd.print(bcdHours & 0x0F); //16 characters poer line
    
   lcd.setCursor(12,1); //we start writing from the first row first column
   lcd.print(":"); //16 characters poer line
     
  
   lcd.setCursor(13,1); //we start writing from the first row first column
   lcd.print(bcdMinutes >> 4); //16 characters poer line
  
   lcd.setCursor(14,1); //we start writing from the first row first column
   lcd.print(bcdMinutes & 0x0F); //16 characters poer line


    delay (5000);
     lcd.clear();//Clean the screen

  }
