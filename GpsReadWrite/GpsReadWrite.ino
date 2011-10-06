#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 12);
bool newgps;
char* rawgps;
char* utctime;
char* longitude;
char* latitude;
char* alt;

char* sats;

#define CH_CR 0x0D 
#define CH_LF 0x0A
const char CRLF[]={CH_CR, CH_LF, 0};

void setup()  
{
  Serial.begin(57600);
  
  //In case the battery died and reset to factory settings change baud rate here
  mySerial.begin(57600);
  char s4[40];
//  mySerial.print(NMEA_ConstructSentence(s4,"PMTK251,14400")); 
//  mySerial.end();
//  mySerial.begin(14400);
  
  char s1[40];
  mySerial.print(NMEA_ConstructSentence(s1,"PMTK220,200")); //200 for 5 HZ 1000 for 1 Hz
  char s2[40];
  mySerial.print(NMEA_ConstructSentence(s2,"PMTK314,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")); //For GGA messages once every 5 messages
  char s3[40];
//  mySerial.print(NMEA_ConstructSentence(s3,"PMTK251,14400")); //Not sure why this has to be sent everytime but won't work without 
   
   
  newgps = false;
  rawgps="";
  utctime = 0;

  free(s4);
  free(s1);
  free(s2);
  free(s3);
}

//
// MAIN LOOP
//
void loop()
{
  if (mySerial.available()) {
    Serial.print((char)mySerial.read());
  }
}


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
