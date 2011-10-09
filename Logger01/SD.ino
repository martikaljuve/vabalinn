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

File dataFile;
void sdOpen(char* file) {
  dataFile = SD.open(file, FILE_WRITE);
}

void sdClose() {
  dataFile.close();
}

void sdWrite(char data) {
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(data);
  }
}

void sdWrite(String data) {
	if (dataFile) {
		dataFile.print(data);
	}
}
