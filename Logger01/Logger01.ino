#include <SoftwareSerial.h>

#define SERIAL_ENABLED 1

void setup() {
  initialize();
  
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
  Serial.begin(115200);
  Serial.println("Serial started.");
#endif

  sdSetup();
}
