/******** Automtic Gardening *****************/
#include <Wire.h>                   //I2C Bus
#include <RTClib.h>                 //Real Time Clock
#include <SD.h>                     //SD Card
#include <LiquidCrystal_I2C.h>      // I2C Lcd Display
#include <dht.h>                    //DHT11 Humidity and Temperadure Sensor

#define deviceAddress 0x68  
#define DHT11_PIN 7                 // Defines DHT11 pins            

dht DHT;
RTC_DS3231 rtc;                     //the object rtc is created from the class RTC_DS3231

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //LCD screen init

File Output_File;

String date;
char filename[] = "LOGGER00.txt";
const int SD_write_ms = 120000;      // Time in ms to Write Output File

const int LCD_refresh_ms = 5000;      // Time in ms to Change LCD Page
int LCD_state = 0;                   // LCD display page
float LCD_time_counter = 0;           // Counts Time to compare with LCD_refresh_ms
float base_LCD_time = 0;             // Zero ref for LCD_time_counter

const int LEDstatus = 3;
const int LEDstop = 5;

const int moisture_sensor = A0;
const int msAirValue = 600;           // Moister sensor Air value
const int msWaterValue = 240;         // Moister sensor Water value
const int Soil_moist_threshold = 350; // Max soil moister threshold to water
int Soil_moisture = 0;                // Variable to check if water is needed
int Soil_moist_percent=0;             // Soil_moisture as percentage

bool Watering_trig = LOW;             // If watering = HIGH -> Water_Garden function turns on the Valve
bool Watering_flag = LOW;             // If watering = HIGH -> Water_Garden function turns on the Valve
bool Valve_state = LOW;               // Valve State (open/closed)
bool  Valve_state_flag= LOW;
const int Valve_Open_ms = 10000;      // Time in ms to let the Valve Open
const int Valve_voltage_thd = 500;      // If Valve_voltage > Valve_voltage_thd Valve is OPEN
int Valve_voltage = 0;                 // Measured Valve Voltage
int Valve_relay = 8;                  // Valve relay pin definition
int base_watering_day = 0;            // Variable to compare if we are in a different day
int base_watering_hour = 0;           // Variable to compare if we are in a different hour
int Water_count_day = 0;              // Counts how many times the garden was watered in the last day
int Water_count_hour = 0;             // Counts how many times the garden was watered in the last day

float SD_write_time_counter = 0;      // Counts how many milli seconds passed since last SD file write
float base_SD_write_time = 0;         // Zero ref for SD_write_time_counter

struct date_time{
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second;
    };

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

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  *date = FAT_DATE(now.year(), now.month(), now.day());       // return date using FAT_DATE macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());   // return time using FAT_TIME macro to format fields
}



bool to_water( uint16_t Soil_moisture, uint16_t Current_day,uint16_t Current_hour){
  if (base_watering_day != Current_day){
    base_watering_day = Current_day;
    Water_count_day = 0;}
    
  if (base_watering_hour != Current_hour){
    base_watering_hour = Current_hour;
    Water_count_hour = 0;}
    
    if (Soil_moisture > Soil_moist_threshold){
        if (Water_count_day < 3 && Water_count_hour < 1){
              Water_count_day = Water_count_day + 1;
              Water_count_hour = Water_count_hour + 1;
              return HIGH;} 
        else return LOW; }
        }

bool WATERING(){
    digitalWrite(Valve_relay, HIGH);
    delay (Valve_Open_ms);
    digitalWrite(Valve_relay, LOW);
  }

bool VALVE_STATE(){
    for (int i = 0; i < 3; i++){
      delay(250);
      Valve_voltage = Valve_voltage + analogRead(A2);
    }
      Valve_voltage = Valve_voltage / 3;
      if (Valve_voltage > Valve_voltage_thd){
        return HIGH;
        }
       else return LOW;

  }

void STOP()
{
 while(1);
}
void setup()
{
  Serial.begin(9600);
  Serial.print("INIT");
  pinMode(10, OUTPUT);
  rtc.begin();
  lcd.begin(16,2); //define LCD:  16 columns and 2 rows
  lcd.backlight(); // LCD Backlight ON
  
  if (!SD.begin(4)) {
    return;
  }
  SdFile::dateTimeCallback(dateTime);

  pinMode(LEDstatus, OUTPUT);
  pinMode(LEDstop, OUTPUT);
  pinMode(moisture_sensor, INPUT);
  pinMode(Valve_relay, OUTPUT);
  
  digitalWrite(Valve_relay, LOW);
  
  date_time now_datetime;
  now_datetime = get_dateTime();
  base_watering_day = now_datetime.day;
    
  for (uint8_t i = 0; i < 100; i++) { //If filename exists increments 1 to filename number
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      Output_File = SD.open(filename, FILE_WRITE);
      Output_File.println("Date,Time,Temperature,Humidity,Soil Moister, Soil Moister Percent,Watering?,Valve State");
      Output_File.close();
      break;}  // leave the loop!
  }    
}

void loop()
{
  delay(5000);
  
  date_time now_datetime;
  now_datetime = get_dateTime();
  
  Output_File = SD.open(filename, FILE_WRITE);
  SD_write_time_counter = millis() - base_SD_write_time;
  
  int Get_Temp_Hum = DHT.read11(DHT11_PIN);
  
  Soil_moisture = analogRead(moisture_sensor);
  Soil_moist_percent = map (Soil_moisture, 240, 620, 0, 100);
 
  Watering_trig = to_water(Soil_moisture, now_datetime.day, now_datetime.hour);
  
  if (Watering_trig == HIGH){
    WATERING();
    Watering_trig = LOW;
    Watering_flag = HIGH;}
    
  Valve_state = VALVE_STATE();

  if (Valve_state == HIGH){
    Valve_state_flag = HIGH;
  }
  
  if (Watering_trig == LOW && Valve_state == HIGH){

    STOP();
  }
  Serial.print(SD_write_time_counter);
  if ( Output_File && (SD_write_time_counter > 120000) ){
    
    base_SD_write_time = millis();
   
    //date
    Output_File.print(now_datetime.year, DEC);
    Output_File.print("/");
    Output_File.print(now_datetime.month, DEC);
    Output_File.print("/");
    Output_File.print(now_datetime.day, DEC);
    Output_File.print(",");
    
    //time
    Output_File.print(now_datetime.hour);
    Output_File.print(':');
    Output_File.print(now_datetime.minute);
    Output_File.print(",");
    
    //sensors
    Output_File.print(DHT.temperature);
    Output_File.print(",");
    Output_File.print(DHT.humidity);
    Output_File.print(",");
    Output_File.print(Soil_moisture);
    Output_File.print(",");
    Output_File.print(Soil_moist_percent);

    //watering

    if (Watering_flag == HIGH){
      Output_File.print(",");
      Output_File.print("1");
      Watering_flag = LOW;}
      
    else{
       Output_File.print(",");
      Output_File.print("0");}

      //Valve State
    if (Valve_state_flag == HIGH){
      Output_File.print(",");
      Output_File.println("1");
      Valve_state_flag = LOW;}
      
    else{
       Output_File.print(",");
      Output_File.println("0");}

  }
  
  Output_File.close();

// Print to LCD \\
  
  LCD_time_counter = millis() - base_LCD_time;

  if (LCD_time_counter >LCD_refresh_ms && LCD_state == 0){
    
    lcd.clear();
    LCD_state = 1;
    base_LCD_time = millis();

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
    lcd.print("C"); }
    
  LCD_time_counter = millis() - base_LCD_time;

  
  if (LCD_time_counter >LCD_refresh_ms && LCD_state == 1){
    
    lcd.clear();//Clean the screen
    LCD_state = 0;
    base_LCD_time = millis();
    
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
    lcd.print(Soil_moisture);
    
    lcd.setCursor(6,1); 
    lcd.print(Soil_moist_percent);
    
    lcd.setCursor(8,1); 
    lcd.print("%");}
}
