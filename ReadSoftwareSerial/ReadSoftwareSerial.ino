#include <SoftwareSerial.h>

SoftwareSerial serial = SoftwareSerial(11, 3);

void setup() {
  serial.begin(57600);
  Serial.begin(115200);
}

char c;
void loop() {
  if (serial.available()) {
    c = serial.read(); 
    if (c == '$')
      Serial.println();
    Serial.write(c);
  }
}
