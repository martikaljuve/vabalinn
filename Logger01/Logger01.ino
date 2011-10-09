#include <SoftwareSerial.h>

#define SERIAL_ENABLED 1

void setup() {
  initialize();
  
  //pinMode(4, OUTPUT);
  //digitalWrite(4, HIGH);
	
  //pinMode(5, OUTPUT);
  //digitalWrite(5, HIGH);
	
  gpsSetup();
  //imuSetup();
}

void loop() {
  gpsLoop();
  //imuLoop();
}

//
// Helper functions
//
void initialize() {
#ifdef SERIAL_ENABLED
  Serial.begin(57600);
  Serial.println("Serial started.");
#endif

  sdSetup();
}
