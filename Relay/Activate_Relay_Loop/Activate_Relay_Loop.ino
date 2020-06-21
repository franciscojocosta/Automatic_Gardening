/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Relay pin is controlled with D8. The active wire is connected to Normally Closed and common
int relay = 8;
volatile byte relayState = LOW;



void setup() {
  // Pin for relay module set as output
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
 
  Serial.begin(9600);
}

void loop() {
  // If 10 seconds have passed, the relay is turned off

    digitalWrite(relay, HIGH);
    delay(5000);
    digitalWrite(relay, LOW);
     delay(5000);

  
  delay(50);
}
