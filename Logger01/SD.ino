#include <SD.h>

const int chipSelect = 10; // Hardware CS pin on most Arduinos

void sdSetup() {
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
#ifdef SERIAL_ENABLED
    Serial.println("Card failed or not present.");
#endif
    return;
  }

#ifdef SERIAL_ENABLED
  Serial.println("Card initialized.");
#endif
}
