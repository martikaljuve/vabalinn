#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial gpsSerial(8, 9);
TinyGPS gps;

const char* gpsFilePattern = "g%02d%02d%02d.txt";
char gpsFile[12];

#define CH_CR 0x0D 
#define CH_LF 0x0A
const char CRLF[]={CH_CR, CH_LF, 0};

void gpsSetup()
{  
  //In case the battery died and reset to factory settings change baud rate here
  gpsSerial.begin(57600);
  char s4[40];
  gpsSerial.print(NMEA_ConstructSentence(s4,"PMTK251,14400")); 
  gpsSerial.end();

  gpsSerial.begin(14400);
  
  char s1[40];
  gpsSerial.print(NMEA_ConstructSentence(s1,"PMTK220,200")); //200 for 5 HZ 1000 for 1 Hz
  char s2[40];
  gpsSerial.print(NMEA_ConstructSentence(s2,"PMTK314,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")); //For GGA messages once every 5 messages
  char s3[40];
  gpsSerial.print(NMEA_ConstructSentence(s3,"PMTK251,14400")); //Not sure why this has to be sent everytime but won't work without 

  free(s1);
  free(s2);
  free(s3);
  free(s4);
}

bool hasInitialized = false;

//
// MAIN LOOP
//
void gpsLoop()
{
  if (hasInitialized) {
    gpsGetData();
  }
  else {
    gpsGetFirstData();
  }
}

void gpsGetFirstData() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
#ifdef SERIAL_ENABLED
    Serial.print(c);
#endif

    if (gps.encode(c)) {
      int year;
      byte month, day, hour, minute, second, hundredths;
      unsigned long fix_age;
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &fix_age);
      
      sprintf(gpsFile, gpsFilePattern, day, hour, minute);
      
      sdOpen("bbb.txt");
      sdWrite('a');
      sdClose();
      
      hasInitialized = true;
#ifdef SERIAL_ENABLED
      Serial.println("GPS first data received:");
      Serial.print("Day: ");
      Serial.print(day);
      Serial.print(", minute: ");
      Serial.print(minute);
      Serial.print(", fix age: ");
      Serial.println(fix_age);
      
      Serial.print("Using GPS file named: ");
      Serial.println(gpsFile);
#endif
    }
  }
}

void gpsGetData() {
  sdOpen(gpsFile);
  
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    sdWrite(c);
#ifdef SERIAL_ENABLED
    Serial.print(c);
#endif
  }
  
  sdClose();
}

//
// HELPER METHODS
//
char * NMEA_ComputeChecksum(char *s) 
{ 
  static char ck_string[4]; 
  static unsigned char chksum;
  for(chksum=0; *s; ++s) 
  {
    chksum ^= *s;
  }
  
  sprintf(ck_string, "*%02X", chksum);

  return(ck_string); 
}

char * NMEA_ConstructSentence(char *s, char *msg ) 
{ 
  strcpy(s, "$"); 
  strcat(s,msg); 
  strcat(s,NMEA_ComputeChecksum(msg)); 
  strcat(s, CRLF);
  return(s); 
}

/*
Supported NMEA Sentences
 0 NMEA_SEN_GLL,     // GPGLL interval - Geographic Position - Latitude longitude  
 1 NMEA_SEN_RMC,     // GPRMC interval - Recommended Minimum Specific GNSS Sentence  
 2 NMEA_SEN_VTG,     // GPVTG interval - Course Over Ground and Ground Speed  
 3 NMEA_SEN_GGA,     // GPGGA interval - GPS Fix Data  
 4 NMEA_SEN_GSA,     // GPGSA interval - GNSS DOPS and Active Satellites  
 5 NMEA_SEN_GSV,     // GPGSV interval - GNSS Satellites in View  
 6 NMEA_SEN_GRS,     // GPGRS interval - GNSS Range Residuals  
 7 NMEA_SEN_GST,     // GPGST interval - GNSS Pseudorange Errors Statistics  
 13 NMEA_SEN_MALM,    // PMTKALM interval - GPS almanac information  
 14 NMEA_SEN_MEPH,    // PMTKEPH interval - GPS ephmeris information  
 15 NMEA_SEN_MDGP,    // PMTKDGP interval - GPS differential correction information  
 16 NMEA_SEN_MDBG,    // PMTKDBG interval – MTK debug information
 
Supported Frequency Setting 
 0 - Disabled or not supported sentence  
 1 - Output once every one position fix  
 2 - Output once every two position fixes  
 3 - Output once every three position fixes  
 4 - Output once every four position fixes  
 5 - Output once every five position fixe
*/
