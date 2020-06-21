
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include "SD.h"

File logfile;
 
RTC_DS1307 rtc;
DateTime now;

/*----------Functions-------------*/
void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while(1);
}

/*--------------------------------*/

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

 
   // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
   if (! logfile) {
    error("couldnt create file");
  }

  // close the file:
    logfile.close();
    Serial.println("done."); 
}

void loop()
{

}
