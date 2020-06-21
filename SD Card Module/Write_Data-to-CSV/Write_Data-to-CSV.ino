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

#define DHT11_PIN 7
#define deviceAddress 0x68
const int stopbutton = 2;
RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231
dht DHT;

File myFile;
File f;

String date;
int i = 0;
void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
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
  
  pinMode(stopbutton, INPUT);
  
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
  Serial.print("Initializing SD card...");
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  SdFile::dateTimeCallback(dateTime);
  
 // myFile = SD.open("test5.txt", FILE_WRITE);
  myFile = SD.open("test5.txt", FILE_WRITE);  
  myFile.println("Date,Time,Temperatura,Humidity");
  
  
}

void loop()
{
  DateTime nowTime = rtc.now();
  int temp = DHT.read11(DHT11_PIN);

 
  // if the file opened okay, write to it:
  if (myFile){
    
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
  Serial.println(i);
  if (i>100){
    myFile.close();
    stop();
  }
  
  if (digitalRead(stopbutton) == HIGH){
    myFile.close();
    stop();
  }
  delay(5000);
  i += 1;
}
