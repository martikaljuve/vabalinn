#include <SD.h>

const int chipSelect = 10; // Hardware CS pin on most Arduinos

void sdSetup() {
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
#ifdef DEBUG
    Serial.println("Card failed, or not present");
#endif
    return;
  }
#ifdef DEBUG
  Serial.println("card initialized.");
#endif
}

File dataFile;
void sdOpen(char* file) {
  dataFile = SD.open(file, FILE_WRITE);
}

void sdClose() {
  dataFile.close();
}

void sdWrite(String dataString) {
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);

#ifdef DEBUG
    Serial.println(dataString);
#endif
  }  
  // if the file isn't open, pop up an error:
  else {
#ifdef DEBUG
    Serial.println("error opening datalog.txt");
#endif
  }
}
